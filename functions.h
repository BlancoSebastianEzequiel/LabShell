#ifndef FUNCTIONS_H
#define FUNCTIONS_H
//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------
#define MAX_BACK 100
//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
#include "types.h"
//------------------------------------------------------------------------------
// EXTERN VARIABLES
//------------------------------------------------------------------------------
extern int status;
//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
int execCommand(struct cmd* cmd);
//------------------------------------------------------------------------------
// EXPAND ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
char* expandEnvironmentVariables(char* arg);
//------------------------------------------------------------------------------
// SET ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
// sets the environment variables passed
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
void setEnvironmentVariables(char** eargv, int eargc);
//------------------------------------------------------------------------------
// RUN BACKGROUND
//------------------------------------------------------------------------------
void runBackground(struct cmd* cmd);
//------------------------------------------------------------------------------
// EXEC BACKGROUND
//------------------------------------------------------------------------------
int execBackground(struct cmd* cmd, pid_t pidChild);
//------------------------------------------------------------------------------
// SEND ACTION SIGNAL
//------------------------------------------------------------------------------
int sendActionSignal();
//------------------------------------------------------------------------------
// OPEN REDIR FD
//------------------------------------------------------------------------------
// opens the file in which the stdin/stdout or
// stderr flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
int openRedirFd(char* file);
//------------------------------------------------------------------------------
// RUN REDIR
//------------------------------------------------------------------------------
void runRedir(struct cmd* cmd);
//------------------------------------------------------------------------------
// RUN MULTIPLE PIPE
//------------------------------------------------------------------------------
void runMultiplePipe(struct cmd* cmd);
//------------------------------------------------------------------------------
// PARSING
//------------------------------------------------------------------------------
int parsing(struct execcmd* c, char* arg);
//------------------------------------------------------------------------------
// GET SIZE
//------------------------------------------------------------------------------
int getSize(char* cmd, char* commands[], int (*f)(char**, char*));
//------------------------------------------------------------------------------
// PRINT COMMAND
//------------------------------------------------------------------------------
int printCommand(char* commands[], size_t size);
//------------------------------------------------------------------------------
// FREE COMMAND
//------------------------------------------------------------------------------
int freeCommand(char* commands[], size_t size);
//------------------------------------------------------------------------------
// GET COMMANDS
//------------------------------------------------------------------------------
char** getCommands(char* cmd, size_t* size);
//------------------------------------------------------------------------------
// CREATE COMMANDS
//------------------------------------------------------------------------------
struct cmd* createCommands(char* cmd);
//------------------------------------------------------------------------------
#endif // FUNCTIONS_H
