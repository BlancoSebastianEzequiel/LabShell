#include <stdlib.h>
#include <unistd.h>
//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
void execCommand(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    int execvp(execcmd->argv[0], execcmd->argv[0] + 1);
}
//------------------------------------------------------------------------------
// EXPAND ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
static char* expandEnvironmentVariables(char* arg) {
    if (arg[0] != '$') return arg;
    char* value = getenv(arg+1);
    if (value == NULL) return arg;
    return value;
}
//------------------------------------------------------------------------------
