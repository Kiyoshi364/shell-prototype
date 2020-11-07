#include  <stdio.h>
#include  <signal.h>

#include "sighand.h"

void handler(int sig) {
	// signal(sig, SIGTSTP);
	printf("Você apertou ctrl Z\n");
	// signal(SIGTSTP, stop_handler);
}

void chld_handler(int sig) {
	printf("Filho terminou ou parou\n");
}

void stop_handler(int sig) {
	// signal(sig, SIGTSTP);
	printf("Você apertou ctrl Z\n");
	// signal(SIGTSTP, stop_handler);
}

void int_handler(int sig) {
	// signal(sig, SIGINT);
	printf("Você apertou ctrl C\n");
	// signal(SIGINT, int_handler);
}

void installSigHandlers() {
	signal(SIGTSTP, stop_handler);
	signal(SIGINT, int_handler);
	signal(SIGCHLD, chld_handler);
}
