#include "builtin.h"

extern int debug;
extern int lastRCode;
extern task_man_t *task_manager;

int getBuiltinID(char *name) {
	int id = 0;

	if (!strcmp(name, "quit")) {
		id = QUIT_BUILTIN;
	} else if (!strcmp(name, "cd")) {
		id = CD_BUILTIN;
	} else if (!strcmp(name, "jobs")) {
		id = JOBS_BUILTIN;
	} else if ( !strcmp(name, "fg") || (*name == '%') ) {
		id = FG_BUILTIN;
	} else if (!strcmp(name, "bg")) {
		id = BG_BUILTIN;
	} else if (!strcmp(name, "tm")) {
		id = TM_BUILTIN;
	}

	return id;
}

int quit(char **argv, char **envp) {
	int rcode = lastRCode, flags = 0;

	// Parsing input
	for (int i = 1; argv[i]; i++) {
		char *arg = argv[i]+1;
		if (argv[i][0] == '-') {
			if ( !strcmp(arg, "-help") ) {
				printf("usage: %s [-f|--force] [n]\n\n", argv[0]);
				printf("\tQuit the shell.\n\n");
				printf("\tExits the shell with a status of [n].\n");
				printf("\tIf [n] is omitted, the exit status is the\n");
				printf("\tsame of the last command runned.\n");
				printf("\tDoes not exit if there are pending tasks,\n");
				printf("\tunless [-f|--force] is used.\n");
				return 0;
			} else if ( !strcmp(arg, "f") || !strcmp(arg,"-force") ) {
				flags |= 1;
			} else {
				printf("Unknown argument (-): %s\n", argv[i]);
				return 1;
			}
		} else if ( !(flags&2) && ('0' <= argv[i][0] && argv[i][0] <= '9') ) {
			int valid = 0;
			rcode = parseInt(argv[i], "", &valid);
			if (!valid) {
				printf("Unknown argument (number): %s\n", argv[i]);
				return 1;
			}
			flags |= 2;
		} else {
			printf("Unknown argument (?): %s\n", argv[i]);
			return 1;
		}
	}

	if ( free_TM(task_manager, 0) ) {
		if ( !(flags&1) ) {
			printf("There are tasks pending.\nUse \"jobs\" to get more info.\n");
			printf("\"quit -f\" to force leaving.\n");

			if (DEBUG_TASKMAN && debug) {
				print_TM(task_manager);
			}
			return 2;
		}
		free_TM(task_manager, 1);
		flags |= 4;
	}

	if (rcode) {
		printf("quit%s (%d).\n", flags&4? " forced": "" , rcode);
	} else {
		printf("quit%s.\n", flags&4? " forced": "" );
	}
	exit(rcode);
}

int cd(char **argv, char **envp) {
	if (!argv[1]) return -1;
	int err;
	if ( (err = chdir(argv[1])) < 1) {
		perror("psh: cd");
	}
	return err;
}

int jobs(char **argv, char **envp) {
	// Parsing input
	for (int i = 1; argv[i]; i++) {
		if ( !strcmp(argv[i], "--help") ) {
			printf("usage: %s\n\n", argv[0]);
			printf("\tPrints all pending tasks.\n");
			return 0;
		} else {
			printf("Unknown argument: %s\n", argv[i]);
			return 1;
		}
	}

	for (int i = 1; i < task_manager->size; i++) {
		task_t *task = (task_manager->tasks)[i];

		if ( task->status != STATUS_RUNNING ) {
			reportTask(task, i);
			continue;
		}

		int status, err = 0;

		if ( (err = waitpid(task->pid, &status, WNOHANG)) < 0 )
			printf("wait jobs: waitpid error (%d).\n", err);

		updateTask(task, status);

		reportTask(task, i);
	}

	return 0;
}

int fg(char **argv, char **envp) {
	int jid = 0, flags = 0;

	if (*argv[0] == '%') {
		int valid = 0;
		jid = parseInt(argv[0]+1, "", &valid);
		if (!valid) {
			printf("Unknown argument (%%): %s\n", argv[0]);
			return 1;
		}
		flags |= jid > 0 ? 2 : 0;
	} else {
		// Parsing input for fg
		for (int i = 1; argv[i]; i++) {
			if ( !strcmp(argv[i], "--help") ) {
				printf("usage: %s [tid]\n\n", argv[0]);
				printf("\tMove task to foreground.\n\n");
				printf("\tIf [tid] is not provided, the\n");
				printf("\tjob if highest tid will be used.\n");
				return 0;
			} else if ( !(flags&2) && ('0' <= argv[i][0] && argv[i][0] <= '9') ) {
				int valid = 0;
				jid = parseInt(argv[i], "", &valid);
				if (!valid) {
					printf("Unknown argument (number): %s\n", argv[i]);
					return 1;
				}
				flags |= 2;
			} else {
				printf("Unknown argument: %s\n", argv[i]);
				return 1;
			}
		}
	}

	task_t *task;

	// validate or find jid
	// if no user input
	if ( !(flags&2) ){
		int i = task_manager->size-1;
		while ( i > 0 ) {
			task = (task_manager->tasks)[i];
			if ( task->status != STATUS_TO_CLEAR ) {
				break;
			}
			i--;
		}
		jid = i;
		if ( !jid ) {
			printf("psh: fg: no pending task.\n");
			return 1;
		}
	} else {
		// if not valid jid
		if ( (jid < 1) || (jid > task_manager->size) ) {
			printf("psh: fg: %d: no such task.\n", jid);
			return 1;
		} else {
			task = (task_manager->tasks)[jid];
			int tstatus = task->status;
			if ( tstatus == STATUS_TO_CLEAR || tstatus == STATUS_NOT_RUNNING ) {
			printf("psh: fg: %d: no such task.\n", jid);
			return 1;
			}
		}
	}

	if (!jid) {
		printf("psh: fg: no pending task.\n");
		return 1;
	}

	// Running
	task_t *temp = *(task_manager->tasks);
	*(task_manager->tasks) = task;

	printf("%s\n", task->cmd);
	if ( task->status == STATUS_STOPPED ) {
		kill(task->pid, SIGCONT);
		task->status = STATUS_RUNNING;
	}

	// Waiting
	int status, err = 0;

	while ( !err ) {
		if ( (err = waitpid(task->pid, &status, WNOHANG | WUNTRACED)) < 0 )
			printf("wait fg foreground: waitpid error (%d).\n", err);
	}

	*(task_manager->tasks) = temp;

	if ( updateTask(task, status) < 0 ) {
		printf("psh: Do not know what happend with the task.\n");
	}

	if ( task->status == STATUS_STOPPED || task->status == STATUS_SIGNALED ) {
		reportTask(task, jid);
	} else if ( task->status == STATUS_DONE || task->status == STATUS_EXITED ) {
		reportTask(task, jid);
	}

	return task->rcode;
}

int bg(char **argv, char **envp) {
	int jid = 0, flags = 0;

	// Parsing input
	for (int i = 1; argv[i]; i++) {
		if ( !strcmp(argv[i], "--help") ) {
			printf("usage: %s [tid]\n\n", argv[0]);
			printf("\tMove task to foreground.\n\n");
			printf("\tIf [tid] is not provided, the\n");
			printf("\tjob if highest tid will be used.\n");
			return 0;
		} else if ( !(flags&2) && ('0' <= argv[i][0] && argv[i][0] <= '9') ) {
			int valid = 0;
			jid = parseInt(argv[i], "", &valid);
			if (!valid) {
				printf("Unknown argument (number): %s\n", argv[i]);
				return 1;
			}
			flags |= 2;
		} else {
			printf("Unknown argument: %s\n", argv[i]);
			return 1;
		}
	}

	task_t *task;

	// validate or find jid
	// if no user input
	if ( !(flags&2) ){
		int i = task_manager->size-1;
		while ( i > 0 ) {
			task = (task_manager->tasks)[i];
			if ( task->status != STATUS_TO_CLEAR ) {
				break;
			}
			i--;
		}
		jid = i;
		if ( !jid ) {
			printf("psh: bg: no pending task.\n");
			return 1;
		}
	} else {
		// if not valid jid
		if ( (jid < 1) || (jid > task_manager->size) ) {
			printf("psh: bg: %d: no such task.\n", jid);
			return 1;
		} else {
			task = (task_manager->tasks)[jid];
			int tstatus = task->status;
			if ( tstatus == STATUS_TO_CLEAR || tstatus == STATUS_NOT_RUNNING ) {
			printf("psh: bg: %d: no such task.\n", jid);
			return 1;
			}
		}
	}

	if (!jid) {
		printf("psh: bg: no pending task.\n");
		return 1;
	}

	// Running
	*(task_manager->tasks) = task;

	printf("%s\n", task->cmd);
	if ( task->status == STATUS_STOPPED ) {
		kill(task->pid, SIGCONT);
		task->status = STATUS_RUNNING;
	}

	return 0;
}

int tm(char **argv, char **envp) {
	print_TM(task_manager);
	return 0;
}

int parseInt(char *str, char *delim, int *valid) {
	int num = 0;

	for (int i = 0; str[i]; i++) {
		char c = str[i];
		for (int j = 0; delim[j]; j++) {
			if (c == delim[j]) {
				*valid = 1;
				return num;
			}
		}

		if ('0' <= c && c <= '9') {
			if ( num*10+c-'0' < num ) {
				*valid = 0;
				return num;
			}
			num = num*10 + c - '0';
		} else {
			*valid = 0;
			return num;
		}
	}

	*valid = 1;
	return num;
}
