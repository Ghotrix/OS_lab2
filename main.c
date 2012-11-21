#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t arr_size(char **array)
{
	size_t size = 0;
	for (array; *array != NULL; array++) {
		size++;
	}

	return size;
}

int main(int argc, char * argv[])
{
	char *app_names[32] = {"cat", "grep", "awk", "sed", "sed", "grep", "wc", NULL};
	char *arg_names[32] = {"log.txt", "10/Oct/2006", "{print $12}", "s/\"//g", "s/\\/.*//g", "-v", "-l"};
	
	int NUM_APPS = arr_size(app_names);
	int fd[256][2], status;
	pid_t child_pids[256];

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

			if(dup2(fd[i][1], 1) < 0)
				fprintf(stderr, "Cannot dup2 stdout...\n");
			
		for (int j = 0; j < NUM_APPS; j++) {
			close(fd[j][0]);
			close(fd[j][1]);
		}

			char* command[4];
			command[0] = app_names[i];
			command[1] = arg_names[i];
			if (i == 5) {
				command[2] = "-";
				command[3] = NULL;
			}
			else {
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

	close(fd[NUM_APPS - 1][1]);
	if (dup2(fd[NUM_APPS - 1][0], 0) < 0)
		fprintf(stderr, "Cannot dup2 stdin...\n");
	close(fd[NUM_APPS - 1][0]);

	int nums;

	scanf("%d\n", &nums);

	/* Очікуємо завершення породжених процесів */
	for (int i = 0; i < NUM_APPS; i++) {
		if (i == NUM_APPS - 1)
			waitpid(child_pids[i], &status, 0);
		else
			waitpid(child_pids[i], NULL, 0);
	}

	app_names[0] = "cat";
	app_names[1] = "grep";
	app_names[2] = "awk";
	app_names[3] = "sed";
	app_names[4] = "sed";
	app_names[5] = "grep";
	app_names[6] = "sort";
	app_names[7] = "uniq";
	app_names[8] = "sort";
	app_names[9] = "head";
	app_names[10]= "awk";
	app_names[11]= NULL;
	
	arg_names[6] = NULL;
	arg_names[7] = "-c";
	arg_names[8] = "-rnk 1";
	arg_names[9] = "-n 10";
	char line[64];
	sprintf(line, "{printf \"%%s - %%d - %%0.f%%\\n\", $2, $1, $1 * 100 / %d}", nums);
	arg_names[10] = line;
	NUM_APPS = arr_size(app_names);

	for (int i = 0; i < NUM_APPS; i++) {
		if(pipe(fd[i]) == -1) {
			fprintf(stderr, "Cannot pipe...\n");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_APPS; i++) {
		child_pids[i] = fork();
		
		if (child_pids[i] == 0) { 
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
			
			char* command[4];
			command[0] = app_names[i];
			command[1] = arg_names[i];
			if (i == 5) {
				command[2] = "-";
				command[3] = NULL;
			}
			else {
				command[2] = NULL;
			}
			execvp(command[0], command);
			_exit(EXIT_FAILURE);
		}
		else if (child_pids[i] == -1) { 
		fprintf(stderr, "Can't fork, exiting...\n");
			_exit(EXIT_FAILURE);
		}
	}
	
	for (int i = 0; i < NUM_APPS; i++) {
		close(fd[i][0]);
		close(fd[i][1]);
	}
	
	for (int i = 0; i < NUM_APPS; i++) {
		if (i == NUM_APPS - 1)
			waitpid(child_pids[i], &status, 0);
		else
			waitpid(child_pids[i], NULL, 0);
	}
	exit(status);
	
	return 0;
}
