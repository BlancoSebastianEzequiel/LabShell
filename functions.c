#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdlib.h>
#include "functions.h"
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
        memcpy(expansion, value, sizeValue);
        expansion[sizeValue] = END_STRING;
        return expansion;
    } else {
        memcpy(arg, value, sizeValue);
        return arg;
    }
}
//------------------------------------------------------------------------------
