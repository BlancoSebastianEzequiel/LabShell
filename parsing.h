#ifndef PARSING_H
#define PARSING_H
//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
#include "defs.h"
#include "types.h"
#include "createcmd.h"
#include "utils.h"
//------------------------------------------------------------------------------
// EXTERN VARIABLES
//------------------------------------------------------------------------------
extern int status;
//------------------------------------------------------------------------------
// PARSE CMD
//------------------------------------------------------------------------------
struct cmd* parse_cmd(char* buf_cmd);
//------------------------------------------------------------------------------
// PARSE LINE
//------------------------------------------------------------------------------
struct cmd* parse_line(char* b);
//------------------------------------------------------------------------------
#endif // PARSING_H
