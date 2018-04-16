#ifndef GENERAL_H
#define GENERAL_H
//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
#include <unistd.h>
#include <dirent.h>
#define BUF_LEN 256
//------------------------------------------------------------------------------
// PERR
//------------------------------------------------------------------------------
// Pre: Recibo un formato y los argumentos
// Pos: Escribo el mensaje por pantalla y se muestra el mensaje de error
// correspondiente a la variable errno
void perr(const char *format, ...);
//------------------------------------------------------------------------------
#endif // GENERAL_H
