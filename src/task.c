#include "task.h"

task_t* newTask(char *cmd, pid_t pid, jid_t jid) {
	task_t *task = malloc(sizeof(*task));
	char *str = malloc(sizeof(*str)*MAXLINE);
	
	int i = -1;
	while (cmd[++i]) str[i] = cmd[i];
	task->cmd = str;

	task->pid = pid;
	task->jid = jid;
	task->running = 0;

	return task;
}

void freeTask(task_t *task) {
	free(task->cmd);
	free(task);
}

task_man_t* init_TM(task_man_t *tm, const unsigned int initLen) {
	tm->tasks = malloc(sizeof(*tm->tasks)*initLen);
	tm->size = 0;
	tm->len = initLen;
	return tm;
}

task_man_t* free_TM(task_man_t *tm, int force) {
	for ( task_t *task ; tm->size > 0; ) {
		task = pop_Task(tm);
		if ( !force && task ) {
			push_Task(tm, task);
			return tm;
		}
	}
	free(tm->tasks);
	free(tm);
	return 0;
}

task_man_t* push_Task(task_man_t *tm, task_t *task) {
	if ( !tm ) return 0;
	if ( tm->size + 1 >= tm->len ) {
		if ( !(tm->tasks = realloc(tm->tasks, tm->len*2)) ) {
			return 0;
		}
	}
	(tm->tasks)[tm->size++] = task;

	return tm;
}

task_t* pop_Task(task_man_t *tm) {
	if ( !tm || tm->size <= 0 ) return 0;
	if ( tm->size - 1 < tm->len/2 ) {
	}
	return (tm->tasks)[--tm->size];
}
