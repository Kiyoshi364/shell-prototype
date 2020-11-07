#ifndef TASK
#define TASK

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "CONSTANTS.h"

typedef struct _TASK {
	char *cmd;
	pid_t pid;
	int status;
	int rcode;
} task_t;

task_t* newTask(char *, pid_t);
void freeTask(task_t *);

int updateTask(task_t *, int);
void reportTask(task_t *task, int jid);

typedef struct _TASK_MANAGER {
	task_t **tasks;
	int size;
	int len;
} task_man_t;

task_man_t* init_TM(const unsigned int);
task_man_t* free_TM(task_man_t *, int);
void print_TM(task_man_t *tm);

int push_Task(task_man_t *, task_t *);
task_t* pop_Task(task_man_t *);

int clean_TM(task_man_t *);

task_t* find_Task(task_man_t *, pid_t, int *);

#endif // TASK
