#define _BSD_SOURCE || _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
#define _POSIX_SOURCE
#define _XOPEN_SOURCE 700
#include "functions.h"
#include "utils.h"
#include "general.h"
#include "printstatus.h"
#include "parsing.h"
#include "exec.h"

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
char scmd[256];
pid_t lastBackPid;
//******************************************************************************
// INICIO FUNCIONES ESTATICAS
//******************************************************************************
//------------------------------------------------------------------------------
// IS EQUAL TO IN N BYTES
//------------------------------------------------------------------------------
static int isEqualToInNBytes(const char* str, const char* cmp, size_t bytes) {
    for (size_t i = 0; i < bytes; ++i) {
        if (str[i] != cmp[i]) return false;
    }
    return true;
}
//------------------------------------------------------------------------------
// FIND FIRS CARACTER AFTER SPACE
//------------------------------------------------------------------------------
static int findFirstCaracterAfterSpace(const char* str, int offset) {
    size_t size = strlen(str);
    for (size_t i = 0; i < size; ++i) {
        if (str[i] == SPACE) continue;
        return i+offset;
    }
    return -1;
}
//------------------------------------------------------------------------------
// GET ENVIRONMENT VALUE
//------------------------------------------------------------------------------
// sets the "value" argument with the value part of
// the "arg" argument and null-terminates it
static void getEnvironmentValue(char* arg, char* value, int idx) {
    int i, j;
    for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++) value[j] = arg[i];
    value[j] = END_STRING;
}
//------------------------------------------------------------------------------
// GET ENVIRONMENT KEY
//------------------------------------------------------------------------------
// sets the "key" argument with the key part of
// the "arg" argument and null-terminates it
static void getEnvironmentKey(char* arg, char* key) {
    int i;
    for (i = 0; arg[i] != '='; i++) key[i] = arg[i];
    key[i] = END_STRING;
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
// PWD
//------------------------------------------------------------------------------
char* getWorkingDirectory() {
    char* directory = (char*) malloc(sizeof(char)* PATH_MAX);
    getcwd(directory, PATH_MAX);
    size_t i = 1;
    while (directory == NULL) {
        free(directory);
        directory = (char*) malloc(sizeof(char)* PATH_MAX*i);
        getcwd(directory, PATH_MAX);
        i++;
    }
    return directory;
}
//------------------------------------------------------------------------------
// SAVE COMMAND
//------------------------------------------------------------------------------
static int saveCommand(char** command, char* left) {
    size_t size = strlen(left);
    *command = (char*) malloc(sizeof(char) * (size + 1));
    if (!*command) {
        printf("ERROR: command = malloc() failed");
        return 1;
    }
    strncpy(*command, left, size+1);
    return 0;
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
// HANDLER
//------------------------------------------------------------------------------
static void handler(int signum, siginfo_t* info, void* context) {
    if (info->si_pid != lastBackPid) return;
    if (lastBackPid == 0 || scmd[0] == END_STRING) return;
    pid_t p = lastBackPid;
    char* f = scmd;
    getMessage(backgroundMsg, 256, "==> terminado: PID=%d (%s)\n", p, f);
    lastBackPid = 0;
    scmd[0] = END_STRING;
}
//******************************************************************************
// FIN FUNCIONES ESTATICAS
//******************************************************************************
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
// EXPAND ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
char* expandEnvironmentVariables(char* arg) {
    if (arg[0] != '$') return arg;

    if (strcmp(arg+1, "?") == 0) {  // Challenge Pseudo-variables
        snprintf(arg, strlen(arg), "%d", status);
        arg[1] = END_STRING;
        return arg;
    }

    char* value = getenv(arg+1);
    if (value == NULL) {
        strcpy(arg, " ");
        return arg;
    }
    size_t sizeArg = strlen(arg);
    size_t sizeValue = strlen(value);
    if (sizeValue > sizeArg) {
        char* expansion = (char*) malloc(sizeof(char)*(sizeValue + 1));
        free(arg);
        strncpy(expansion, value, sizeValue);
        expansion[sizeValue] = END_STRING;
        return expansion;
    } else {
        strncpy(arg, value, sizeValue);
        arg[sizeValue] = END_STRING;
        return arg;
    }
}
//------------------------------------------------------------------------------
// CD
//------------------------------------------------------------------------------
int changeDirectory(char* cmd) {
    if (!isEqualToInNBytes(cmd, "cd", 2)) return false;
    int idx = findFirstCaracterAfterSpace(cmd+2, 2);
    int value;
    if (idx == -1) {
        value = chdir("/home");
    } else {
        value = chdir(cmd+idx);
    }
    char* directory = getWorkingDirectory();
    strncpy(promt, directory, strlen(directory));
    free(directory);
    if (value == -1) perr("ERROR function chdir() failed");
    return true;
}
//------------------------------------------------------------------------------
// EXIT
//------------------------------------------------------------------------------
int exitNicely(char* cmd) {
    if (!isEqualToInNBytes(cmd, "exit", 4)) return false;
    int status = 0;
    exit(status & 0377);
    return true;
}
//------------------------------------------------------------------------------
// PWD
//------------------------------------------------------------------------------
int printWorkingDirectory(char* cmd) {
    if (!isEqualToInNBytes(cmd, "pwd", 3)) return false;
    char* directory = getWorkingDirectory();
    printf("%s\n", directory);
    free(directory);
    return true;
}
//------------------------------------------------------------------------------
// SET ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
void setEnvironmentVariables(char** eargv, int eargc) {
    for (size_t i = 0; i < eargc; ++i) {
        int idx = block_contains(eargv[i], '=');
        if (idx == -1) continue;
        size_t size = strlen(eargv[i]);
        char* key = (char*) malloc(sizeof(char) * (idx + 1));
        if (key == NULL) continue;
        char* value = (char*) malloc(sizeof(char) * (size - idx));
        if (key == NULL) {
            free(key);
            continue;
        }
        getEnvironmentKey(eargv[i], key);
        getEnvironmentValue(eargv[i], value, idx);
        if (setenv(key, value, 0) == -1) {
            perr("ERROR: function setenv(%s, %s, 0) returned -1", key, value);
        }
        free(key);
        free(value);
    }
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
    lastBackPid = pidChild;
    size_t size = strlen(cmd->scmd);
    strncpy(scmd, cmd->scmd, size);
    print_back_info(cmd);
    return true;
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
// RUN REDIR
//------------------------------------------------------------------------------
void runRedir(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;

    int fdIn = openRedirFd(execcmd->in_file);
    int fdOut = openRedirFd(execcmd->out_file);
    int fdErr = openRedirFd(execcmd->err_file);

    int redirIn = redir(fdIn, STDIN_FILENO);
    int redirOut = redir(fdOut, STDOUT_FILENO);
    int redirErr = redir(fdErr, STDERR_FILENO);

    if (redirIn == -1 || redirOut == -1 || redirErr == -1) exitNicely("exit");
    cmd->type = EXEC;
    exec_cmd(cmd);
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
// PARSING
//------------------------------------------------------------------------------
int parsing(struct execcmd* c, char* arg) {
    size_t size = strlen(arg);
    size_t append = 0;
    size_t redirErrOut = 0;
    for (size_t i = 0; i < size; ++i) {
        if (arg[i] == '>' && (i == 0 || i == 1)) append++;
        if (arg[i] == '&' && i == 0) redirErrOut++;
    }

    if (redirErrOut == 1 && append == 1) {
        strcpy(c->out_file, &arg[2]);
        strcpy(c->err_file, &arg[2]);
        free(arg);
        c->type = REDIR;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
// GET SIZE
//------------------------------------------------------------------------------
int getSize(char* cmd, char* commands[], int (*f)(char**, char*)) {
    int i = 0;
    char* aux = (char*) malloc(sizeof(char) * (strlen(cmd) + 1));
    strcpy(aux, cmd);
    char* left = aux;
    char* right;
    while (block_contains(left, '|') >= 0) {
        right = split_line(left, '|');
        if (f) f(&commands[i], left);
        left = right;
        i++;
    }
    if (f) f(&commands[i], left);
    free(aux);
    return ++i;
}
//------------------------------------------------------------------------------
// PRINT COMMAND
//------------------------------------------------------------------------------
int printCommand(char* commands[], size_t size) {
    printf("{ ");
    for (size_t j = 0; j < size; ++j) {
        if (j == size-1) printf("%s", commands[j]);
        else printf("%s, ", commands[j]);
    }
    printf(" }\n");
    return 0;
}
//------------------------------------------------------------------------------
// FREE COMMAND
//------------------------------------------------------------------------------
int freeCommand(char* commands[], size_t size) {
    for (size_t j = 0; j < size; ++j) free(commands[j]);
    free(commands);
    return 0;
}
//------------------------------------------------------------------------------
// GET COMMANDS
//------------------------------------------------------------------------------
char** getCommands(char* cmd, size_t* size) {
    *size = getSize(cmd, NULL, NULL);
    char** commands = (char**) malloc(sizeof(char*) * (*size + 1));
    if (!commands) {
        perr("ERROR: commands = malloc() failed");
        return NULL;
    }
    commands[*size] = NULL;
    getSize(cmd, commands, saveCommand);
    return commands;
}
//------------------------------------------------------------------------------
// CREATE COMMANDS
//------------------------------------------------------------------------------
struct cmd* createCommands(char* cmd) {
    size_t size;
    char** commands = getCommands(cmd, &size);
    size_t posSize = sizeof(struct cmd*);
    struct cmd** cmdVec = (struct cmd**) malloc(posSize*(size+1));
    if (!cmdVec) {
        perr("ERROR: cmdVec = malloc() failed");
        freeCommand(commands, size);
        return NULL;
    }
    for (size_t i = 0; i < size; ++i) cmdVec[i] = parse_cmd(commands[i]);
    freeCommand(commands, size);
    cmdVec[size] = NULL;
    return pipe_cmd_create(cmdVec, size);
}
//------------------------------------------------------------------------------
