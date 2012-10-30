#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//#define NUM_PIPES 4
#define NUM_APPS 5//NUM_PIPES + 1

int main(int argc, char * argv[])
{
	int fd[NUM_PIPES][2], status;
	char *app_names[] = {"cat", "grep", "awk", "sed", "sed", "grep"};
	char *arg_names[] = {"log.txt", "10/Oct/2006", "{print $12}", "s/\"//g", "s/\\/.*//g", "1"};
	pid_t child_pids[NUM_APPS];

	for (int i = 0; i < NUM_PIPES; i++) {
		if(pipe(fd[i]) == -1) {
			fprintf(stderr, "Cannot pipe...\n");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_APPS; i++) {
		child_pids[i] = fork();
		
		if (child_pids[i] == 0) { /* We're in child process */
			if (i != 0) {
				close(fd[i-1][1]);
				if(dup2(fd[i-1][0], 0) < 0)
					fprintf(stderr, "Cannot dup2 stdin...\n");
				close(fd[i-1][0]);
			}
			if (i != NUM_APPS - 1) {
				close(fd[i][0]);
				if(dup2(fd[i][1], 1) < 0)
					fprintf(stderr, "Cannot dup2 stdout...\n");
				if (i != NUM_PIPES - 1)
					close(fd[i][1]);
			}
			
		for (int j = 0; j < NUM_PIPES; j++) {
			close(fd[j][0]);
			if (i != NUM_APPS - 1 && j != NUM_PIPES - 1)
				close(fd[j][1]);
		}

			char* command[3] = {app_names[i], arg_names[i], NULL};
			execvp(command[0], command);
			_exit(EXIT_FAILURE);
		} 
		else if (child_pids[i] == -1) { /* Cannot fork */
			fprintf(stderr, "Can't fork, exiting...\n");
			_exit(EXIT_FAILURE);
		}
	}

	/*

	close(fd[3][1]);
	if (dup2(fd[3][0], 0) < 0)
		fprintf(stderr, "Cannot dup2 stdin...\n");
	close(fd[3][0]);

	char buf[1024];

	while (scanf("%s\n", buf) != EOF) {
		printf("%s\n", buf);
	}

	*/

	for (int i = 0; i < NUM_PIPES; i++) {
		close(fd[i][0]);
		close(fd[i][1]);
	}

	/* Це батьківський процес */
	/* Очікуємо завершення породжених процесів */
	for (int i = 0; i < NUM_APPS; i++) {
		if (i == NUM_APPS - 1)
			waitpid(child_pids[i], &status, 0);
		else
			waitpid(child_pids[i], NULL, 0);
	}

	/* Завершуємося з кодом повернення grep */
	exit(status);
	
	return 0;
}
