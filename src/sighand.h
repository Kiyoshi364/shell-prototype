#ifndef SIGHAND
#define SIGHAND

#include <stdio.h>
#include <sys/wait.h>

#include "task.h"

extern void print_cli();

void chld_handler(int);
void stop_handler(int);
void int_handler(int);
void installSigHandlers();

#endif // SIGHAND
