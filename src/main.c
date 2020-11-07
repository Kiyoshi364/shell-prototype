#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "CONSTANTS.h"
#include "sighand.h"
#include "task.h"
#include "builtin.h"

// Globals
int debug = 1;
int events = 0;
task_man_t *task_manager;

// Main Functions
void print_cli();
int readCmd(char *, int *);
int parseCmd(char *, char *, char **);
int run(char *, int, char **, char **, int);
int builtin(char **, char **);

// Char functions
int validHex(char);
char hexToNum(char);

// To implement
void show_events();

int main(int argc, char **argv, char **envp) {

	// install handlers
	installSigHandlers();

	// init task manager
	task_manager = init_TM(INITIAL_STACK_SIZE);
	{
	task_t *fg_task = newTask("", 0);
	fg_task->status = STATUS_NOT_RUNNING;
	push_Task(task_manager, fg_task);
	}

	// for reading cmd
	char cmd[MAXLINE] = "";
	int cli = 1, read = 0;

	// for parsing cmd
	char *argv2[MAXARGS];
	char buffer[MAXLINE];

	// for running in background
	int bg = 0;

	while (1) {

		if (cli) print_cli();

		// Read from stdin
		cli = readCmd(cmd, &read);

		// Parse cmd
		bg = parseCmd(cmd, buffer, argv2);

		// Do command or built-in
		run(cmd, read, argv2, envp, bg);

		// Check for events
		if (events) show_events();
	}

	return 0;
}

void print_cli(int breakLine) {
	printf("%c  >", breakLine ? '\0' : '\n');
	fflush(stdout);
}

int readCmd(char *cmd, int *lenp) {
	int read = 0, cli = 0, endOfLine = 0;
	while (!endOfLine && read < MAXLINE) {
		int c = fgetc(stdin);

		switch (c) {
			case EOF:
				putchar('\n');
				exit(0);

			case '\n':
				cli = 1;
				endOfLine = 1;
				break;

			case '&':
				cmd[read++] = c;
			case ';':
				endOfLine = 1;
				break;

			case ' ':
				if (!read || cmd[read-1] == ' ') break;

			default:
				cmd[read++] = c;
		}
	}
	cmd[read] = '\0';

	*lenp = read + 1;

	if (DEBUG_READ && debug && *cmd) printf("Read: %s\n\n", cmd);

	return cli;
}

int parseCmd(char *cmd, char *buffer, char **argv) {
	int bg = 0;

	int argc = 0, ci = 0, bi = 0;
	int state = JUST_FINISHED_STATE;

	while (cmd[ci]) {
		if (state == JUST_FINISHED_STATE) {
			argv[argc++] = buffer + bi;
			state = NORMAL_STATE;
		}

		char c = cmd[ci++];

		if (state == NORMAL_STATE) {
			switch (c) {
				case '\'':
					// Copy until next '
					state = COPY_SIMPLE_STATE;
					break;
				case '"':
					// Copy and parse until next "
					state = COPY_DOUBLE_STATE;
					break;
				case '\\':
					// Escaping sequence '\'
					state |= ESCAPE_STATE;
					break;

				case '&':
					bg = 1;
				case ' ':
					state = JUST_FINISHED_STATE;
					buffer[bi++] = '\0';
					break;

				default:
					buffer[bi++] = c;
					break;
			}

		} else if (state == COPY_SIMPLE_STATE) {
			// Back to the normal
			if (c == '\'') state = NORMAL_STATE;

			else buffer[bi++] = c;

		} else if (state == COPY_DOUBLE_STATE) {
			// Back to the normal
			if (c == '"') state = NORMAL_STATE;

			// Escaping sequence '\'
			else if (c == '\\') state |= ESCAPE_STATE;

			else buffer[bi++] = c;

		} else if (state & ESCAPE_STATE) {

			char n1, n2;
			switch (c) {
				case 'n':
					buffer[bi++] = '\n';
					break;
				case '\'':
					buffer[bi++] = '\'';
					break;
				case '"':
					buffer[bi++] = '"';
					break;
				case '\\':
					buffer[bi++] = '\\';
					break;
				case 'x':
					n1 = cmd[ci++], n2 = cmd[ci++];
					if ( validHex(n1) && validHex(n2) ) {
						buffer[bi++] = hexToNum(n1)*0x10 + hexToNum(n2);
						break;
					}
					ci -= 2;
				default:
					buffer[bi++] = c;
			}

			state &= ~ESCAPE_STATE;

		} else {
			// Should NOT enter here
			buffer[bi++] = '\0';
			printf("AAAAAAAAAAAAAAA PANIC AAAAAAAAAAAAAAA\n");
			printf("Parser entered in an undefined state.\n");
			printf("cmd: %s\n", cmd);
			printf("bg: %d, ci: %d, bi: %d\n", bg, ci, bi);
			printf("argc: %d\n", argc);
			for (int i = 0; i < argc; i++) {
				printf("argv[%d]: %s\n", i, argv[i]);
			}
			printf("AAAAAAAAAAAAAAA PANIC AAAAAAAAAAAAAAA\n");
			exit(1);
		}
	}
	buffer[bi++] = '\0';
	argv[argc] = buffer + bi;
	buffer[bi++] = '\0';

	argc = bg ? argc-1 : argc;

	if (DEBUG_PARSE && debug) {
		printf("cmd: %s\n", cmd);
		printf("bg: %d\n", bg);
		printf("argc: %d\n", argc);
		for (int i = 0; i < argc; i++) {
			printf("argv[%d]: %s\n", i, argv[i]);
		}
		putchar('\n');
	}

	return bg;
}

int run(char *cmd, int len, char **argv, char **envp, int bg) {
	if (!(*cmd) || len <= 1) return 0;

	pid_t pid;
	int jid = 0, rcode;

	int id = getBuiltinID(*argv), forked = bg || !id;

	// Running de command
	if ( (forked) && ((pid = fork()) == 0) ) {

		if ( !id ) {
			if (execve(argv[0], argv, envp) < 0)
				printf("%s: command not found.\n", argv[0]);
			exit(0);
		}
		rcode = builtin(argv, envp);
		exit(rcode);
	}

	if (!forked) {
		rcode = builtin(argv, envp);
	}

	// Update Task Manager
	task_t *task = newTask(cmd, pid);
	task->status = STATUS_RUNNING;

	if (bg) {
		jid = push_Task(task_manager, task);

		printf("[%u] %d\t%s\n", jid, pid, cmd);
	} else if (forked) {
		task_t *temp = *(task_manager->tasks);
		*(task_manager->tasks) = task;

		int status, err;
		if ( (err = waitpid(pid, &status, WUNTRACED)) < 0 )
			printf("wait foreground: waitpid error (%d).\n", err);

		updateTask(task, status);

		*(task_manager->tasks) = temp;

		if (task->status == STATUS_EXITED) {
			// do nothing
		} else if ( task->status == STATUS_STOPPED || task->status == STATUS_SIGNALED ) {
			jid = push_Task(task_manager, task);
			events = 1;
		}
	} else { // Builtin
		// TODO: use rcode
	}

	if (!jid) freeTask(task);

	if (DEBUG_TASKMAN && debug) {
		print_TM(task_manager);
	}

	return 0;
}

int builtin(char **argv, char **envp) {
	int id = 0, rcode;

	id = getBuiltinID(*argv);

	switch (id) {
		case QUIT_BUILTIN:
			rcode = quit(argv, envp);
			break;
		case CD_BUILTIN:
			rcode = cd(argv, envp);
			break;
		case JOBS_BUILTIN:
			rcode = jobs(argv, envp);
			break;
		case FG_BUILTIN:
			rcode = fg(argv, envp);
			break;
		case BG_BUILTIN:
			rcode = bg(argv, envp);
			break;

		default:
			rcode = UNKNOWN_BUILTIN;
		case UNKNOWN_BUILTIN:
			break;
	}

	if (DEBUG_BUILTIN && debug) {
		printf("builtin_id: %d, rcode: %d\n\n", id, rcode);
	}

	return rcode;
}

void show_events() {
	int status;
	pid_t pid;
	int jid = 0;

	while ( (pid = waitpid(-1, &status, 0)) > 0 ) {
		task_t *task = find_Task(task_manager, pid, &jid);
		if ( !task ) {
			// Should NOT enter here
			printf("AAAAAAAAAAAAAAA PANIC AAAAAAAAAAAAAAA\n");
			print_TM(task_manager);
			printf("AAAAAAAAAAAAAAA PANIC AAAAAAAAAAAAAAA\n");
			continue;
		}
		updateTask(task, status);
		reportTask(task, jid);
		// printf("Filho pid=%d: terminou ou parou (%d)\n", pid, status); // DEBUG_CODE
	}
	// printf("LAST: Filho pid=%d: terminou ou parou (%d)\n", pid, status); // DEBUG_CODE

	 clean_TM(task_manager);

	events = 0;

	if (DEBUG_TASKMAN && debug) {
		print_TM(task_manager);
	}
}

int validHex(char c) {
	return ('0' <= c && c <= '9') || 
		('a' <= c && c <= 'f') ||
		('A' <= c && c <= 'F');
}

char hexToNum(char c) {
	char off = 0;
	off = '0' <= c && c <= '9' ? '0' : 0 ;
	off = 'a' <= c && c <= 'f' ? 'a' - 10 : off ;
	off = 'A' <= c && c <= 'F' ? 'A' - 10 : off ;
	return c - off;
}
