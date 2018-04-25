#include "parsing.h"
#include "general.h"
//------------------------------------------------------------------------------
// GET TOKEN
//------------------------------------------------------------------------------
// parses an argument of the command stream input
static char* get_token(char* buf, int idx) {

	char* tok;
	int i;

	tok = (char*)calloc(ARGSIZE, sizeof(char));
	i = 0;

	while (buf[idx] != SPACE && buf[idx] != END_STRING) {
		tok[i] = buf[idx];
		i++; idx++;
	}

	return tok;
}
//------------------------------------------------------------------------------
// PARSE REDIR FLOW
//------------------------------------------------------------------------------
// parses and changes stdin/out/err if needed
static bool parse_redir_flow(struct execcmd* c, char* arg) {

	int inIdx, outIdx;

	// Challenge Parte 2: Flujo estándar
	size_t size = strlen(arg);
	size_t append = 0;
	size_t redirErrOut = 0;
	for (size_t i = 0; i < size; ++i) {
		if (arg[i] == '>' && (i == 0 || i == 1)) append++;
		if (arg[i] == '&' && i == 0) redirErrOut++;
	}

	if (redirErrOut == 1 && append == 1) {
		strcpy(c->out_file, &arg[2]);
		strcpy(c->err_file, &arg[2]);
		free(arg);
		c->type = REDIR;
		return true;
	}

	// flow redirection for output
	if ((outIdx = block_contains(arg, '>')) >= 0) {
		switch (outIdx) {
			// stdout redir
			case 0: {
				strcpy(c->out_file, arg + 1);
				break;
			}
			// stderr redir
			case 1: {
				strcpy(c->err_file, &arg[outIdx + 1]);
				break;
			}
		}
		
		free(arg);
		c->type = REDIR;
		
		return true;
	}
	
	// flow redirection for input
	if ((inIdx = block_contains(arg, '<')) >= 0) {
		// stdin redir
		strcpy(c->in_file, arg + 1);
		
		c->type = REDIR;
		free(arg);
		
		return true;
	}
	
	return false;
}
//------------------------------------------------------------------------------
// PARSE ENVIRON VAR
//------------------------------------------------------------------------------
// parses and sets a pair KEY=VALUE
// environment variable
static bool parse_environ_var(struct execcmd* c, char* arg) {

	// sets environment variables apart from the
	// ones defined in the global variable "environ"
	if (block_contains(arg, '=') > 0) {

		// checks if the KEY part of the pair
		// does not contain a '-' char which means
		// that it is not a environ var, but also
		// an argument of the program to be executed
		// (For example:
		// 	./prog -arg=value
		// 	./prog --arg=value
		// )
		if (block_contains(arg, '-') < 0) {
			c->eargv[c->eargc++] = arg;
			return true;
		}
	}
	
	return false;
}
//------------------------------------------------------------------------------
// EXPAND ENVIRONMENT VARIABLES
//------------------------------------------------------------------------------
// this function will be called for every token, and it should
// expand environment variables. In other words, if the token
// happens to start with '$', the correct substitution with the
// environment value should be performed. Otherwise the same
// token is returned.
//
// Hints:
// - check if the first byte of the argument
// 	contains the '$'
// - expand it and copy the value
// 	 to 'arg' 
static char* expand_environ_var(char* arg) {
	// Your code here
	if (arg[0] != '$') return arg;

	if (strcmp(arg+1, "?") == 0) {  // Challenge Pseudo-variables
		snprintf(arg, strlen(arg), "%d", status);
		arg[1] = END_STRING;
		return arg;
	}

	char* value = getenv(arg+1);
	if (value == NULL) {
		strcpy(arg, " ");
		return arg;
	}
	size_t sizeArg = strlen(arg);
	size_t sizeValue = strlen(value);
	if (sizeValue > sizeArg) {
		char* expansion = (char*) malloc(sizeof(char)*(sizeValue + 1));
		free(arg);
		strncpy(expansion, value, sizeValue);
		expansion[sizeValue] = END_STRING;
		return expansion;
	} else {
		strncpy(arg, value, sizeValue);
		arg[sizeValue] = END_STRING;
		return arg;
	}
}
//------------------------------------------------------------------------------
// PARSE EXEC
//------------------------------------------------------------------------------
// parses one single command having into account:
// - the arguments passed to the program
// - stdin/stdout/stderr flow changes
// - environment variables (expand and set)
static struct cmd* parse_exec(char* buf_cmd) {

	struct execcmd* c;
	char* tok;
	int idx = 0, argc = 0;
	
	c = (struct execcmd*)exec_cmd_create(buf_cmd);
	
	while (buf_cmd[idx] != END_STRING) {
	
		tok = get_token(buf_cmd, idx);
		idx = idx + strlen(tok);
		
		if (buf_cmd[idx] != END_STRING)
			idx++;
		
		if (parse_redir_flow(c, tok))
			continue;
		
		if (parse_environ_var(c, tok))
			continue;

		tok = expand_environ_var(tok);
		
		c->argv[argc++] = tok;
	}
	
	c->argv[argc] = (char*)NULL;
	c->argc = argc;
	
	return (struct cmd*)c;
}
//------------------------------------------------------------------------------
// PARSE BACK
//------------------------------------------------------------------------------
// parses a command knowing that it contains
// the '&' char
static struct cmd* parse_back(char* buf_cmd) {

	int i = 0;
	struct cmd* e;

	while (buf_cmd[i] != '&')
		i++;
	
	buf_cmd[i] = END_STRING;
	
	e = parse_exec(buf_cmd);

	return back_cmd_create(e);
}
//------------------------------------------------------------------------------
// PARSE CMD
//------------------------------------------------------------------------------
// parses a command and checks if it contains
// the '&' (background process) character
struct cmd* parse_cmd(char* buf_cmd) {

	if (strlen(buf_cmd) == 0)
		return NULL;
		
	int idx;

	// checks if the background symbol is after
	// a redir symbol, in which case
	// it does not have to run in in the 'back'
	if ((idx = block_contains(buf_cmd, '&')) >= 0 && buf_cmd[idx - 1] != '>' &&
			buf_cmd[idx + 1] != '>')
		return parse_back(buf_cmd);
		
	return parse_exec(buf_cmd);
}
//------------------------------------------------------------------------------
// GET SIZE
//------------------------------------------------------------------------------
static int getSize(char* cmd, char* commands[], int (*f)(char**, char*)) {
	int i = 0;
	char* aux = (char*) malloc(sizeof(char) * (strlen(cmd) + 1));
	strcpy(aux, cmd);
	char* left = aux;
	char* right;
	while (block_contains(left, '|') >= 0) {
		right = split_line(left, '|');
		if (f) f(&commands[i], left);
		left = right;
		i++;
	}
	if (f) f(&commands[i], left);
	free(aux);
	return ++i;
}
//------------------------------------------------------------------------------
// FREE COMMAND
//------------------------------------------------------------------------------
static int freeCommand(char* commands[], size_t size) {
	for (size_t j = 0; j < size; ++j) free(commands[j]);
	free(commands);
	return 0;
}
//------------------------------------------------------------------------------
// SAVE COMMAND
//------------------------------------------------------------------------------
static int saveCommand(char** command, char* left) {
	size_t size = strlen(left);
	*command = (char*) malloc(sizeof(char) * (size + 1));
	if (!*command) {
		printf("ERROR: command = malloc() failed");
		return 1;
	}
	strncpy(*command, left, size+1);
	return 0;
}
//------------------------------------------------------------------------------
// GET COMMANDS
//------------------------------------------------------------------------------
static char** getCommands(char* cmd, size_t* size) {
	*size = getSize(cmd, NULL, NULL);
	char** commands = (char**) malloc(sizeof(char*) * (*size + 1));
	if (!commands) {
		perr("ERROR: commands = malloc() failed");
		return NULL;
	}
	commands[*size] = NULL;
	getSize(cmd, commands, saveCommand);
	return commands;
}
//------------------------------------------------------------------------------
// CREATE COMMANDS
//------------------------------------------------------------------------------
static struct cmd* createCommands(char* cmd) {
	size_t size;
	char** commands = getCommands(cmd, &size);
	size_t posSize = sizeof(struct cmd*);
	struct cmd** cmdVec = (struct cmd**) malloc(posSize*(size+1));
	if (!cmdVec) {
		perr("ERROR: cmdVec = malloc() failed");
		freeCommand(commands, size);
		return NULL;
	}
	for (size_t i = 0; i < size; ++i) cmdVec[i] = parse_cmd(commands[i]);
	freeCommand(commands, size);
	cmdVec[size] = NULL;
	return pipe_cmd_create(cmdVec, size);
}
//------------------------------------------------------------------------------
// PARSE LINE
//------------------------------------------------------------------------------
// parses the command line
// looking for the pipe character '|'
struct cmd* parse_line(char* buf) {
	return createCommands(buf);
}
//------------------------------------------------------------------------------
