#include "runcmd.h"
#include "functions.h"
int status = 0;
struct cmd* parsed_pipe;
char backgroundMsg[256];
// runs the command in 'cmd'
int run_cmd(char* cmd) {
	
	pid_t p;
	struct cmd *parsed;

	// if the "enter" key is pressed
	// just print the promt again
	if (cmd[0] == END_STRING)
		return 0;

	// cd built-in call
	if (cd(cmd))
		return 0;

	// exit built-in call
	if (exit_shell(cmd))
		return EXIT_SHELL;

	// pwd buil-in call
	if (pwd(cmd))
		return 0;

	// parses the command line
	parsed = parse_line(cmd);
	struct execcmd* execcmd = (struct execcmd*) parsed;
	setEnvironmentVariables(execcmd->eargv, execcmd->eargc);
	
	// forks and run the command
	if ((p = fork()) == 0) {

		// keep a reference
		// to the parsed pipe cmd
		// so it can be freed later
		if (parsed->type == PIPE)
			parsed_pipe = parsed;

		exec_cmd(parsed);
	}
	// store the pid of the process
	parsed->pid = p;

	// background process special treatment
	// Hint:
	// - check if the process is
	// 	going to be run in the 'back'
	// - print info about it with
	// 	'print_back_info()'
	//
	// Your code here
	// waits for the process to finish
	if (!execBackground(parsed, p)) {
		waitpid(p, &status, 0);
		printf("%s\n", backgroundMsg);
		backgroundMsg[0] = END_STRING;
	}

	print_status_info(parsed);
	free_command(parsed);

	return 0;
}

