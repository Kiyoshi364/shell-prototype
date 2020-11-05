#include <stdio.h>
#include <stdlib.h>

#include "CONSTANTS.h"

// Globals
int verbose = 1;
int events = 0;

// Functions
void print_cli();
int eval(char *);

void show_events();

int main(int argc, char **argv, char **envp) {

	char cmd[MAXLINE] = "";
	int read = 0, cli = 1, endOfLine = 0;

	while (1) {

		// Check for events TODO
		if (events) show_events();

		if (verbose && *cmd) printf("Read: %s\n", cmd);

		// Do command
		eval(cmd);

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
				case ';':
					endOfLine = 1;
					break;

				default:
					cmd[read++] = c;
			}
		}
		cmd[read] = '\0';
	}

	return 0;
}

void print_cli(int breakLine) {
	printf("%c  >", breakLine ? '\0' : '\n');
	fflush(stdout);
}

int eval(char *cmd) {
	if (!(*cmd)) return 0;

	return 0;
}

void show_events() {
}
