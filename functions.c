//------------------------------------------------------------------------------
// EXEC COMMAND
//------------------------------------------------------------------------------
void execCommand(struct cmd* cmd) {
    struct execcmd* execcmd = (struct execcmd*) cmd;
    int execvp(execcmd->argv[0], execcmd->argv[0] + 1);
    
}
//------------------------------------------------------------------------------
