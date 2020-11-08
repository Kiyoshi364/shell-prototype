#include "task.h"

task_t* newTask(char *cmd, pid_t pid) {
	task_t *task = malloc(sizeof(*task));
	char *str = malloc(sizeof(*str)*MAXLINE);
	
	int i = -1;
	while (cmd[++i]) str[i] = cmd[i];
	task->cmd = str;

	task->pid = pid;
	task->status = 0;

	return task;
}

void freeTask(task_t *task) {
	free(task->cmd);
	free(task);
}

int updateTask(task_t *task, int status) {
	if (WIFEXITED(status)) {
		task->rcode = WEXITSTATUS(status);
		task->status = !(task->rcode) ? STATUS_DONE : STATUS_EXITED;

	} else if (WIFSTOPPED(status)) {
		task->rcode = WSTOPSIG(status);
		task->status = STATUS_STOPPED;

	} else if (WIFSIGNALED(status)) {
		task->rcode = WTERMSIG(status);
		task->status = STATUS_SIGNALED;

	} else {
		// Undefined State
		task->status = STATUS_NOT_RUNNING;
		return -1;
	}
	return 0;
}

void reportTask(task_t *task, int jid) {
	pid_t pid = task->pid;
	int rcode = task->rcode;
	char *cmd = task->cmd;
	switch (task->status) {
		case STATUS_RUNNING:
			/* constant 0 may be a FUTURE CHANGE, to implement '+', '-', 0 */
			printf("[%d]%c %d Running\t\t%s\n", jid, 0, pid, cmd);
			break;
		case STATUS_STOPPED:
			printf("[%d]%c %d Stopped by %d\t\t%s\n", jid, 0, pid, rcode, cmd);
			break;
		case STATUS_SIGNALED:
			printf("[%d]%c %d Signaled by %d\t\t%s\n", jid, 0, pid, rcode, cmd);
			task->status = STATUS_TO_CLEAR;
			break;
		case STATUS_EXITED:
			printf("[%d]%c %d Exited with %d\t\t%s\n", jid, 0, pid, rcode, cmd);
			task->status = STATUS_TO_CLEAR;
			break;
		case STATUS_DONE:
			printf("[%d]%c %d Done\t\t%s\n", jid, 0, pid, cmd);
			task->status = STATUS_TO_CLEAR;
			break;
		case STATUS_TO_CLEAR:
			break;
		default:
			printf("[%d]%c %d Unknown\t\t%s\n", jid, 0, pid, cmd);
			task->status = STATUS_TO_CLEAR;
			break;
	}
}

task_man_t* init_TM(const unsigned int initLen) {
	task_man_t *tm = malloc(sizeof(*tm));
	tm->tasks = malloc(sizeof(*tm->tasks)*initLen);
	tm->size = 0;
	tm->len = initLen;
	return tm;
}

task_man_t* free_TM(task_man_t *tm, int force) {
	for ( task_t *task ; tm->size; ) {
		task = pop_Task(tm);
		if ( !force && (task->status != STATUS_TO_CLEAR) ) {
			if (tm->size > 0) {
				push_Task(tm, task);
				return tm;
			}
		}
	}
	free(tm->tasks);
	free(tm);
	return 0;
}

void print_TM(task_man_t *tm) {
	if ( !tm ) return;

	printf("Task Manager");
	printf("\tSize: %d, Len: %d {\n", tm->size, tm->len);
	for (int i = 0; i < tm->size; i++) {
		task_t *task = tm->tasks[i];
		printf("\tTask %d:\n", i);
		printf("\t\tcmd: %s\n", task->cmd);
		printf("\t\tpid: %d, status: %d, rcode: %d\n",
			task->pid, task->status, task->rcode);
	}
	puts("}\n");
}

int push_Task(task_man_t *tm, task_t *task) {
	if ( !tm ) return 0;
	if ( tm->size + 1 >= tm->len ) {
		if ( !(tm->tasks = realloc(tm->tasks, tm->len*2)) ) {
			return 0;
		}
		tm->len *= 2;
	}

	int jid = tm->size;

	(tm->tasks)[tm->size++] = task;

	return jid;
}

task_t* pop_Task(task_man_t *tm) {
	if ( !tm || tm->size <= 0 ) return 0;
	if ( (tm->len > INITIAL_STACK_SIZE) && (tm->size - 1 < tm->len/2) ) {
		if ( !(tm->tasks = realloc(tm->tasks, tm->len/2)) ) {
			return 0;
		}
		tm->len /= 2;
	}
	return (tm->tasks)[--tm->size];
}

int clean_TM(task_man_t *tm) {
	task_t *task;
	while (tm->size > 1) {
		task = pop_Task(tm);

		if (task->status != STATUS_TO_CLEAR) {
			push_Task(tm, task);
			break;
		}

		freeTask(task);
	}
	return tm->size;
}

task_t* find_Task(task_man_t *tm, pid_t pid, int *jidp) {
	task_t *task;
	for (int i = 0; i < tm->size; i++) {
		task = (tm->tasks)[i];
		if (task->pid == pid) {
			*jidp = i;
			return task;
		}
	}
	return 0;
}
