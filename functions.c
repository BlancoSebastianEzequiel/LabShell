//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
void execCommand(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    char* arguments[] = {NULL};
    int execvp(execcmd->argv[0], arguments);
}
//------------------------------------------------------------------------------
