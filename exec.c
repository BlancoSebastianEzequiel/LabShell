#include "exec.h"
#include "functions.h"
#include "general.h"
//------------------------------------------------------------------------------
// GET ENVIRON KEY
//------------------------------------------------------------------------------
// sets the "key" argument with the key part of
// the "arg" argument and null-terminates it
static void get_environ_key(char* arg, char* key) {
	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}
//------------------------------------------------------------------------------
// GET ENVIRON VALUE
//------------------------------------------------------------------------------
// sets the "value" argument with the value part of
// the "arg" argument and null-terminates it
static void get_environ_value(char* arg, char* value, int idx) {
	int i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++) {
		value[j] = arg[i];
	}
	value[j] = END_STRING;
}
//------------------------------------------------------------------------------
// SET ENVIRON VARS
//------------------------------------------------------------------------------
// sets the environment variables passed
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void set_environ_vars(char** eargv, int eargc) {
	// Your code here
	for (size_t i = 0; i < eargc; ++i) {
		int idx = block_contains(eargv[i], '=');
		if (idx == -1) continue;
		size_t size = strlen(eargv[i]);
		char* key = (char*) malloc(sizeof(char) * (idx + 1));
		if (key == NULL) continue;
		char* value = (char*) malloc(sizeof(char) * (size - idx));
		if (key == NULL) {
			free(key);
			continue;
		}
		get_environ_key(eargv[i], key);
		get_environ_value(eargv[i], value, idx);
		if (setenv(key, value, 0) == -1) {
			perr("ERROR: function setenv(%s, %s, 0) returned -1", key, value);
		}
		free(key);
		free(value);
	}
}
//------------------------------------------------------------------------------
// EXEC CMD
//------------------------------------------------------------------------------
// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
void exec_cmd(struct cmd* cmd) {

	switch (cmd->type) {

		case EXEC: {
			// spawns a command
			struct execcmd* execcmd = (struct execcmd*) cmd;
			set_environ_vars(execcmd->eargv, execcmd->eargc);
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

