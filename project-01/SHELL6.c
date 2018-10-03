
#define COMMAND_PROMPT "myshell> "
#define COMMAND_MAX_LENGTH 1024

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

bool is_exit_command(char *command);
void execute_command(char *command);
int main(int argc, char const *argv[]) {
	/* Create buffer for command */
	char command[COMMAND_MAX_LENGTH];

	/* Endless loop, only stop looping when exit command is executed */
	while(1) {
		/* Print prompt */
		printf(COMMAND_PROMPT);

		/* Read command and cut off newline*/
		fgets(command, COMMAND_MAX_LENGTH, stdin);
		command[strlen(command) - 1] = 0;

		/* Check if the command is exit */
		if(is_exit_command(command)) {
			break;
		}

		/* Execute command */
		execute_command(command);
	}

	return 0;
}

bool is_exit_command(char *command) {
	return strcmp(command, "exit") == 0;
}

void execute_command(char *command) {
	/* Fork process */
	pid_t pid = fork();

	/* Parent code */
	if(pid > 0) {
		/* Wait for child to terminate */
		wait(NULL);
	}

	/* Child code */
	if(pid == 0) {
		/* Create dummy args */
		char *args[2];
		args[0] = command;
		args[1] = NULL;

		/* Load new program */
		execvp(command, args);

		/* If this code gets executed, execlp failed */
		printf("ERROR: No such command.\n");
		exit(1);

	}

	/* Error handling */
	if(pid < 0) {
		printf("ERROR: fork() failed.\n");
	}
}