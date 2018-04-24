#include "exec.h"
#include "functions.h"
// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
void exec_cmd(struct cmd* cmd) {

	switch (cmd->type) {

		case EXEC: {
			// spawns a command
			if (!execCommand(cmd)) { // Your code here
				free_command(cmd);
				exit(0);
			}
			printf("Commands are not yet implemented\n");
			_exit(-1);
			break;
		}

		case BACK: {
			// runs a command in background
			runBackground(cmd);  //Your code here
			printf("Background process are not yet implemented\n");
			_exit(-1);
			break;
		}

		case REDIR: {
			// changes the input/output/stderr flow
			runRedir(cmd); // Your code here
			printf("Redirections are not yet implemented\n");
			_exit(-1);
			break;
		}
		
		case PIPE: {
			// pipes two commands
			runMultiplePipe(cmd); // Your code here
			printf("Pipes are not yet implemented\n");
				
			// free the memory allocated
			// for the pipe tree structure
			free_command(parsed_pipe);

			break;
		}
	}
}

