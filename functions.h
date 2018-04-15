#ifndef LABSHELL_H
#define LABSHELL_H
//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
#include "types.h"
//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
void execCommand(struct cmd* cmd);
//------------------------------------------------------------------------------
// EXPAND ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
char* expandEnvironmentVariables(char* arg);
//------------------------------------------------------------------------------
#endif // LABSHELL_H