#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "CONSTANTS.h"

// Globals
int debug = 1;
int events = 0;

// Functions
void print_cli();
int eval(char *, char **);
int builtin(char **, int);

// Char functions
int validHex(char);
char hexToNum(char);

// To implement
void show_events();

int main(int argc, char **argv, char **envp) {

	char cmd[MAXLINE] = "";
	int read = 0, cli = 1, endOfLine = 0;

	while (1) {

		// Do command or built-in
		eval(cmd, envp);

		// Check for events TODO
		if (events) show_events();

		if (cli) print_cli();

		// Read from stdin
		read = 0, cli = 0, endOfLine = 0;
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

		if (DEBUG_READ && debug && *cmd) printf("Read: %s\n\n", cmd);
	}

	return 0;
}

void print_cli(int breakLine) {
	printf("%c  >", breakLine ? '\0' : '\n');
	fflush(stdout);
}

int eval(char *cmd, char **envp) {
	if (!(*cmd)) return 0;

	char *argv[MAXARGS];
	char buffer[MAXLINE];
	int bg = 0;

	int argc = 0, ci = 0, bi = 0;
	int state = JUST_FINISHED_STATE;

	// Parsing
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

	pid_t pid;
	int jid, status;

	// Running de command
	if (!builtin(argv, bg)) {
		if ((pid = fork()) == 0) {
			if (execve(argv[0], argv, envp) < 0) {
				printf("%s: Command not found.\n", argv[0]);
				exit(0);
			}
		}

		// TODO: Update Task Manager
		jid = 0;


		if (bg) {
			printf("[%u] %d\t\t%s\n", jid, pid, cmd);
		} else {
			int err;
			if ( (err = waitpid(pid, &status, WUNTRACED)) < 0 )
				printf("wait foreground: waitpid error (%d).\n", err);
		}
	}

	return 0;
}

int builtin(char **argv, int bg) {
	return 0;
}

void show_events() {
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
