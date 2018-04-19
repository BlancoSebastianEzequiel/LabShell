#ifndef CREATECMD_H
#define CREATECMD_H

#include "defs.h"
#include "types.h"

struct cmd* exec_cmd_create(char* cmd);

struct cmd* back_cmd_create(struct cmd* c);

struct cmd* pipe_cmd_create(struct cmd** cmdVec, size_t size);

#endif // CREATECMD_H
