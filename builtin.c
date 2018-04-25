#include "builtin.h"
#include "general.h"
#include <unistd.h>
#include <stdlib.h>
//------------------------------------------------------------------------------
// IS EQUAL TO IN N BYTES
//------------------------------------------------------------------------------
static int isEqualToInNBytes(const char* str, const char* cmp, size_t bytes) {
	for (size_t i = 0; i < bytes; ++i) {
		if (str[i] != cmp[i]) return false;
	}
	return true;
}
//------------------------------------------------------------------------------
// FIND FIRS CARACTER AFTER SPACE
//------------------------------------------------------------------------------
static int findFirstCaracterAfterSpace(const char* str, int offset) {
	size_t size = strlen(str);
	for (size_t i = 0; i < size; ++i) {
		if (str[i] == SPACE) continue;
		return i+offset;
	}
	return -1;
}
//------------------------------------------------------------------------------
// GET WORKING DIRECTORY
//------------------------------------------------------------------------------
static char* getWorkingDirectory() {
	char* directory = (char*) malloc(sizeof(char)* PATH_MAX);
	getcwd(directory, PATH_MAX);
	size_t i = 2;
	while (directory == NULL) {
		free(directory);
		directory = (char*) malloc(sizeof(char)* PATH_MAX*i);
		getcwd(directory, PATH_MAX);
		i++;
	}
	return directory;
}
//------------------------------------------------------------------------------
// EXIT SHELL
//------------------------------------------------------------------------------
// returns true if the 'exit' call
// should be performed
int exit_shell(char* cmd) {
	// Your code here
	if (!isEqualToInNBytes(cmd, "exit", 4)) return false;
	int status = 0;
	exit(status & 0377);
	return true;
}
//------------------------------------------------------------------------------
// CD
//------------------------------------------------------------------------------
// returns true if "chdir" was performed
// this means that if 'cmd' contains:
// 	$ cd directory (change to 'directory')
// 	$ cd (change to HOME)
// it has to be executed and then return true
int cd(char* cmd) {
	// Your code here
	if (!isEqualToInNBytes(cmd, "cd", 2)) return false;
	int idx = findFirstCaracterAfterSpace(cmd+2, 2);
	int value;
	if (idx == -1) {
		value = chdir("/home");
	} else {
		value = chdir(cmd+idx);
	}
	char* directory = getWorkingDirectory();
	getMessage(promt, PRMTLEN, "(%s)", directory);
	free(directory);
	if (value == -1) perr("ERROR function chdir() failed");
	return true;
}
//------------------------------------------------------------------------------
// PWD
//------------------------------------------------------------------------------
// returns true if 'pwd' was invoked
// in the command line
int pwd(char* cmd) {
	// Your code here
	if (!isEqualToInNBytes(cmd, "pwd", 3)) return false;
	char* directory = getWorkingDirectory();
	printf("%s\n", directory);
	free(directory);
	return true;
}
//------------------------------------------------------------------------------
