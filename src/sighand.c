#include  <stdio.h>
#include  <signal.h>

#include "sighand.h"

extern int events;
extern task_man_t *task_manager;

void handler(int sig) {
	task_t *fg_task = *(task_manager->tasks);
	pid_t pid = fg_task->pid;
	if (pid == 0) {
		if (sig == SIGINT) print_cli();
		return;
	}

	int err;
	if ( (err = kill(pid, sig)) < 0 )
		printf("Could not send sig (%d) to pid (%d), err (%d).\n", sig, pid, err);

	events = 1;
}

void chld_handler(int sig) {
	events = 1;
}

void installSigHandlers() {
	signal(SIGTSTP, handler);
	signal(SIGINT, handler);
	signal(SIGCHLD, chld_handler);
}
