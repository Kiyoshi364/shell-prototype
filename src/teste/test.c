#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp) {
	char *argv2[] = {"./psh"};

	if (execve(argv2[0], argv2, __environ) < 0) {
		system(argv2[0]);
		printf("%s: Command not found.\n", argv2[0]);
	}
}
