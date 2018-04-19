#ifndef PARSING_H
#define PARSING_H

#include "defs.h"
#include "types.h"
#include "createcmd.h"
#include "utils.h"
struct cmd* parse_cmd(char* buf_cmd);
struct cmd* parse_line(char* b);

#endif // PARSING_H
