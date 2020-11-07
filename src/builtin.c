#include "builtin.h"

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

int quit(char **argv, char **env) {
	return 0;
}

int cd(char **argv, char **env) {
	return 0;
}

int jobs(char **argv, char **env) {
	return 0;
}

int fg(char **argv, char **env) {
	return 0;
}

int bg(char **argv, char **env) {
	return 0;
}
