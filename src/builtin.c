#include "builtin.h"

extern int debug;
extern task_man_t *task_manager;

int getBuiltinID(char *name) {
	int id = 0;

	if (!strcmp(name, "quit")) {
		id = QUIT_BUILTIN;
	} else if (!strcmp(name, "cd")) {
		id = CD_BUILTIN;
	} else if (!strcmp(name, "jobs")) {
		id = JOBS_BUILTIN;
	} else if (!strcmp(name, "fg")) {
		id = FG_BUILTIN;
	} else if (!strcmp(name, "bg")) {
		id = BG_BUILTIN;
	}

	return id;
}

int quit(char **argv, char **envp) {
	int rcode = 0, flags = 0;

	// Parsing input
	for (int i = 1; argv[i]; i++) {
		char *arg = argv[i]+1;
		if (argv[i][0] == '-') {
			if ( !strcmp(arg, "-help") ) {
				printf("usage: %s [-f|--force] [n]\n\n", argv[0]);
				printf("\tQuit the shell.\n\n");
				printf("\tExits the shell with a status of [n].\n");
				printf("\tIf [n] is omitted, the exit status is 0.\n");
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
	return 0;
}

int jobs(char **argv, char **envp) {
	return 0;
}

int fg(char **argv, char **envp) {
	return 0;
}

int bg(char **argv, char **envp) {
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
