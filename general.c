#include "general.h"
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
//------------------------------------------------------------------------------
// PERR
//------------------------------------------------------------------------------
void perr(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char msgError[BUF_LEN];
    vsnprintf(msgError, BUF_LEN, format, args);
    va_end(args);
    perror(msgError);
}
//------------------------------------------------------------------------------
