#include "functions.h"
#include "utils.h"
#include "general.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
void execCommand(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    const char* file = (const char*) execcmd->argv[0];
    execvp(file, execcmd->argv);
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
