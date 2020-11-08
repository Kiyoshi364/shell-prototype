#ifndef BUILTIN
#define BUILTIN

#include <string.h>

#include "task.h"

#define UNKNOWN_BUILTIN		0
#define QUIT_BUILTIN		1
#define CD_BUILTIN			2
#define JOBS_BUILTIN		3
#define FG_BUILTIN			4
#define BG_BUILTIN			5

int getBuiltinID(char *);

int quit(char **, char **);
int cd(char **, char **);
int jobs(char **, char **);
int fg(char **, char **);
int bg(char **, char **);

int parseInt(char *, char *, int *);

#endif // BUILTIN
