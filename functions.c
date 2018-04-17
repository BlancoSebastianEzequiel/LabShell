#include "functions.h"
#include "utils.h"
#include "general.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include "exec.h"
#include "printstatus.h"
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
static void getEnvironmentValue(char* arg, char* value, int idx) {
    int i, j;
    for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++) value[j] = arg[i];
    value[j] = END_STRING;
}
//------------------------------------------------------------------------------
// GET ENVIRONMENT KEY
//------------------------------------------------------------------------------
static void getEnvironmentKey(char* arg, char* key) {
    int i;
    for (i = 0; arg[i] != '='; i++) key[i] = arg[i];
    key[i] = END_STRING;
}
//------------------------------------------------------------------------------
// MOVE COMMAND
//------------------------------------------------------------------------------
void moveCommand(struct cmd* cmdDest, struct cmd* cmdSrc) {
    cmdDest->type = cmdSrc->type;
    cmdDest->pid = cmdSrc->pid;
    strncpy(cmdDest->scmd, cmdSrc->scmd, strlen(cmdSrc->scmd));
}
//******************************************************************************
// FIN FUNCIONES ESTATICAS
//******************************************************************************
//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
void execCommand(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    if (execcmd->argc == 0) return;
    const char* file = (const char*) execcmd->argv[0];
    if (execvp(file, execcmd->argv) == -1) {
        perr("ERROR function execvp() returned -1");
    }
}
//------------------------------------------------------------------------------
// EXPAND ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
char* expandEnvironmentVariables(char* arg) {
    if (arg[0] != '$') return arg;
    char* value = getenv(arg+1);
    if (value == NULL) return arg;
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
    char* directory = (char*) malloc(sizeof(char)* PATH_MAX);
    getcwd(directory, PATH_MAX);
    size_t i = 1;
    while (directory == NULL) {
        free(directory);
        directory = (char*) malloc(sizeof(char)* PATH_MAX*i);
        getcwd(directory, PATH_MAX);
        i++;
    }
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
int execBackground(struct cmd* cmd) {
    if (cmd->type != BACK) return false;
    print_back_info(cmd);
    return true;
}
//------------------------------------------------------------------------------
