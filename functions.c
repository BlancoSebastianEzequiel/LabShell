#define _BSD_SOURCE || _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
#define _POSIX_SOURCE
#define _XOPEN_SOURCE 700
#include "functions.h"
#include "utils.h"
#include "general.h"
#include "printstatus.h"
#include "parsing.h"
#include "exec.h"
#include "builtin.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
//------------------------------------------------------------------------------
// EXTERNAL VARIABLE
//------------------------------------------------------------------------------
pid_t pidBack[MAX_BACK];
char* scmdBack[MAX_BACK];
size_t posBack = 0;
//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
int execCommand(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    if (execcmd->argc == 0) return false;
    const char* file = (const char*) execcmd->argv[0];
    if (execvp(file, execcmd->argv) == -1) {
        perr("ERROR function execvp() returned -1");
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
// RUN BACKGROUND
//------------------------------------------------------------------------------
void runBackground(struct cmd* cmd) {
    struct backcmd* backcmd = (struct backcmd*) cmd;
    exec_cmd(backcmd->c);
}
//------------------------------------------------------------------------------
// EXEC BACKGROUND
//------------------------------------------------------------------------------
int execBackground(struct cmd* cmd, pid_t pidChild) {
    if (cmd->type != BACK) return false;
    pidBack[posBack] = pidChild;
    size_t size = strlen(cmd->scmd);
    scmdBack[posBack] = (char*) malloc(sizeof(char) * (size+1));
    strncpy(scmdBack[posBack], cmd->scmd, size+1);
    posBack++;
    // Vuelvo al principio ya que a medida que imprimo por pantalla se va
    // liberando memoria y guardando punteros a null asique no habria
    // corrupcion de memoria
    if (posBack >= MAX_BACK) posBack = 0;
    print_back_info(cmd);
    return true;
}
//------------------------------------------------------------------------------
// HANDLER
//------------------------------------------------------------------------------
static void handler(int signum, siginfo_t* info, void* context) {
    pid_t p;
    char* f;
    int position;
    int found = false;
    for (size_t i = 0; i < MAX_BACK; ++i) {
        if (info->si_pid != pidBack[i]) continue;
        p = pidBack[i];
        f = scmdBack[i];
        position = i;
        found = true;
        break;
    }
    if (!found) return;
    getMessage(backgroundMsg, 256, "==> terminado: PID=%d (%s)\n", p, f);
    pidBack[position] = -1;
    free(scmdBack[position]);
}
//------------------------------------------------------------------------------
// SEND ACTION SIGNAL
//------------------------------------------------------------------------------
int sendActionSignal() {
    struct sigaction act;
    // act.sa_handler = handler;
    act.sa_sigaction = handler;
    act.sa_flags = SA_RESTART;
    // act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        perr("ERROR: sigaction() failed in function sendActionSignal");
        return 1;
    }
    return 0;
}
//------------------------------------------------------------------------------
// OPEN REDIR FD
//------------------------------------------------------------------------------
int openRedirFd(char* file) {
    size_t size = strlen(file);
    if (size == 0) return -1;
    if (size == 1 && isdigit(file)) return (int) *file;
    int idx = block_contains(file, '&');
    if (idx >= 0 && size == 2) return atoi(file + idx + 1);
    int append = false;
    if (file[0] == '>') append = true;  // Challenge Parte 2: Flujo estándar

    int fd;
    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  // 0644
    if (append) {
        fd = open(&file[1], O_RDWR | O_CREAT | O_APPEND, mode);
    } else {
        fd = open(file, O_RDWR | O_CREAT, mode);
    }
    if (fd == -1) {
        perr("ERROR: open failed with file %s in function openRedirFd()", file);
    }
    return fd;
}
//------------------------------------------------------------------------------
// REDIR
//------------------------------------------------------------------------------
static int redir(int oldFd, int newFd) {
    if (newFd == -1 || oldFd == -1) return 0;
    int fd = dup2(oldFd, newFd);
    if (fd == -1) perr("ERROR: dup2(%d, %d) filed", oldFd, newFd);
    return fd;
}
//------------------------------------------------------------------------------
// RUN REDIR
//------------------------------------------------------------------------------
void runRedir(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;

    int fdIn = openRedirFd(execcmd->in_file);
    int fdOut = openRedirFd(execcmd->out_file);
    int fdErr;
    if (strcmp(execcmd->out_file, execcmd->err_file) != 0) {
        fdErr = openRedirFd(execcmd->err_file);
    }
    else {
        fdErr = 1;
    }

    int redirIn = redir(fdIn, STDIN_FILENO);
    int redirOut = redir(fdOut, STDOUT_FILENO);
    int redirErr = redir(fdErr, STDERR_FILENO);

    if (redirIn == -1 || redirOut == -1 || redirErr == -1) exit_shell("exit");
    cmd->type = EXEC;
    exec_cmd(cmd);
}
//------------------------------------------------------------------------------
// RUN PIPE
//------------------------------------------------------------------------------
static void runPipe(struct cmd* left, struct cmd* right, int last) {
    int pipeFd[2];
    if (pipe(pipeFd) == -1) perr("ERROR: pipe() failed in function runPipe()");
    // if (redir(pipeFd[0], pipeFd[1]) == -1) perr("ERROR redir failed");

    int status = 0;

    pid_t p = fork();
    if (p == -1) perr("ERROR fork failed in function runPipe()");
    if (p == 0) {
        // child process
        close(pipeFd[0]); // Close unused write end
        if (redir(pipeFd[1], STDOUT_FILENO) == -1) perr("ERROR redir failed");
        exec_cmd(left);
    } else {
        // parent process
        waitpid(p, &status, 0);
        close(pipeFd[1]); // Close unused read end
        if (redir(pipeFd[0], STDIN_FILENO) == -1) perr("ERROR redir failed");
        if (last) exec_cmd(right);
    }
}
//------------------------------------------------------------------------------
// RUN MULTIPLE PIPE
//------------------------------------------------------------------------------
void runMultiplePipe(struct cmd* cmd) {
    struct pipecmd* pipecmd = (struct pipecmd*) cmd;
    int last = false;
    for (size_t i = 0; i < pipecmd->size-1; ++i) {
        if (i == pipecmd->size-2) last = true;
        runPipe(pipecmd->cmdVec[i], pipecmd->cmdVec[i+1], last);
    }
}
//------------------------------------------------------------------------------
