#include "builtin.h"
#include "functions.h"

// returns true if the 'exit' call
// should be performed
int exit_shell(char* cmd) {
	return exitNicely(cmd);  // Your code here
}

// returns true if "chdir" was performed
// this means that if 'cmd' contains:
// 	$ cd directory (change to 'directory')
// 	$ cd (change to HOME)
// it has to be executed and then return true
int cd(char* cmd) {
	return changeDirectory(cmd);  // Your code here
}

// returns true if 'pwd' was invoked
// in the command line
int pwd(char* cmd) {
	return printWorkingDirectory(cmd);  // Your code here
}

