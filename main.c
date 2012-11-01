#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "map.h"

int main(int argc, char * argv[])
{
	char *app_names[] = {"cat", "grep", "awk", "sed", "sed"};
	char *arg_names[] = {"log.txt", "10/Oct/2006", "{print $12}", "s/\"//g", "s/\\/.*//g"};
	int NUM_APPS = sizeof(app_names) / sizeof(app_names[0]);
	int fd[NUM_APPS][2], status;
	pid_t child_pids[NUM_APPS]; /* Array containing pids of children processes */

	for (int i = 0; i < NUM_APPS; i++) {
		if(pipe(fd[i]) == -1) {
			fprintf(stderr, "Cannot pipe...\n");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_APPS; i++) {
		child_pids[i] = fork();
		
		if (child_pids[i] == 0) { /* We're in child process */
			if (i != 0) {
				if(dup2(fd[i-1][0], 0) < 0) /* Make taking of input from pipe */
					fprintf(stderr, "Cannot dup2 stdin...\n");
			}

			if(dup2(fd[i][1], 1) < 0) /* Make flowing of output to pipe */
				fprintf(stderr, "Cannot dup2 stdout...\n");
			
		for (int j = 0; j < NUM_APPS; j++) {
			close(fd[j][0]);
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
	
	for (int i = 0; i < NUM_APPS - 1; i++) {
		close(fd[i][0]);
		close(fd[i][1]);
	}

	close(fd[NUM_APPS - 1][1]);
	if (dup2(fd[NUM_APPS - 1][0], 0) < 0)
		fprintf(stderr, "Cannot dup2 stdin...\n");
	close(fd[NUM_APPS - 1][0]);
	
	/* Очікуємо завершення породжених процесів */
	for (int i = 0; i < NUM_APPS; i++) {
		if (i == NUM_APPS - 1)
			waitpid(child_pids[i], &status, 0);
		else
			waitpid(child_pids[i], NULL, 0);
	}

	if (status == EXIT_FAILURE)
		exit(EXIT_FAILURE);

	char buf[1024];

	struct map_t *browsers = map_create();
	
	unsigned int items = 0;

	while(scanf("%s\n", buf) != EOF) {
		items++;
		int *value_p = map_get_value(browsers, buf, strlen(buf) + 1);
		int value;
		if (value_p == NULL)
			value = 1;
		else
			value = *value_p + 1;
		map_set(browsers, buf, strlen(buf) + 1, &value, sizeof(int));
	}

	unsigned int map_size = map_get_size(browsers);

	char *string = malloc(1024);
	int it = 0;
	size_t string_len = 0;
	size_t string_alloc = 1024;

	for (struct map_t *map = browsers; map != NULL; map = map->nxt, it++) {
		if (string_alloc <= string_len) {
			string_alloc *= 2;
			string = realloc(string, string_alloc);
		}

		sprintf(string+string_len, "%s - %d - %.0f%\n", (char *)map->key, *(int *)map->value, floor((*(int *)map->value) * 100. / items));
		string_len = strlen(string);
	}

	map_destruct(browsers);

	char *b_app_names[] = {"echo", "sort", "head"};
	char *b_arg_names[] = {"-e", "-nrk 3", "-n 10"};

	NUM_APPS = sizeof(b_app_names) / sizeof (b_app_names[0]);

	for (int i = 0; i < NUM_APPS; i++) {
		if(pipe(fd[i]) == -1) {
			fprintf(stderr, "Cannot pipe...\n");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_APPS; i++) {
		child_pids[i] = fork();
		
		if (child_pids[i] == 0) { /* We're in child process */
			if (i != 0) {
				if(dup2(fd[i-1][0], 0) < 0)
					fprintf(stderr, "Cannot dup2 stdin...\n");
			}
			if (i != NUM_APPS - 1) {
				if(dup2(fd[i][1], 1) < 0)
					fprintf(stderr, "Cannot dup2 stdout...\n");
			}

			for (int j = 0; j < NUM_APPS; j++) {
				close(fd[j][0]);
				close(fd[j][1]);
			}
			
			char *command[4];

			if (i == 0) {
				command[0] = b_app_names[i];
				command[1] = b_arg_names[i];
				command[2] = string;
				command[3] = NULL;
			}
			else {
				command[0] = b_app_names[i];
				command[1] = b_arg_names[i];
				command[2] = NULL;
			}
			execvp(command[0], command);
			_exit(EXIT_FAILURE);
		}
		else if (child_pids[i] == -1) { /* Cannot fork */
			fprintf(stderr, "Can't fork, exiting...\n");
			_exit(EXIT_FAILURE);
		}
	}
	
	for (int i = 0; i < NUM_APPS - 1; i++) {
		close(fd[i][0]);
		close(fd[i][1]);
	}
	
	/* Очікуємо завершення породжених процесів */
	for (int i = 0; i < NUM_APPS; i++) {
		if (i == NUM_APPS - 1)
			waitpid(child_pids[i], &status, 0);
		else
			waitpid(child_pids[i], NULL, 0);
	}
	
	/* Завершуємося з кодом повернення останнього процесу */
	exit(status);
	
	return 0;
}
