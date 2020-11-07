#ifndef TASK
#define TASK

#include <unistd.h>
#include<stdlib.h>

#include "CONSTANTS.h"

typedef int jid_t;

typedef struct _TASK {
	char *cmd;
	pid_t pid;
	jid_t jid;
	int running;
} task_t;

task_t* newTask(char *, pid_t, jid_t);
void freeTask(task_t *);

typedef struct _TASK_MANAGER {
	task_t **tasks;
	int size;
	int len;
} task_man_t;

task_man_t* init_TM(task_man_t *, const unsigned int);
task_man_t* free_TM(task_man_t *, int);

task_man_t* push_Task(task_man_t *, task_t *);
task_t* pop_Task(task_man_t *);

#endif // TASK
