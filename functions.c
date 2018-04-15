#include <stdlib.h>
#include <unistd.h>
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
    return value;
}
//------------------------------------------------------------------------------
