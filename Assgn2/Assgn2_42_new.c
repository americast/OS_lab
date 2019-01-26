#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include<fcntl.h>

void execute(char* exec, int len, int fd[2], int pipe_in)
{
	printf("exec: %s\n", exec);
	int count = 0, actual_i, pipe_out = 0;
	char here[100];
	char* args[100];
	for(actual_i = 0; ;actual_i++)
	{
		sscanf(exec+count, "%s", here);
		count+=strlen(here);
		for (;*(exec+count)==' ';)
			count++;
		if(strcmp(here,"|")==0)
		{
			args[actual_i] = NULL;
			pipe_out = 1;
			break;
		}
		else
		{
			args[actual_i] = (char *) malloc(strlen(here)+1);
			strcpy(args[actual_i], here);
		}
		if (count>=len)
		{
			args[++actual_i] = NULL;
			break;
		}
	}

	pid_t p = fork();

	if (p < 0)
	{
		perror("Unable to fork");
	}
	else if (p > 0 && pipe_out)  // parent
	{
		printf("I am waiting\n");
		wait(NULL);
		printf("I am done waiting\n");
		execute(exec+count, len-count, fd, pipe_out);
		int j;
		for (j = 0; j < actual_i; j++)
			free(args[j]);
	}
	else if(p > 0)
	{
		wait(NULL);
	}
	else if (p==0)	// child
	{
		if (pipe_in && pipe_out)
		{
			close(0);
			dup2(fd[0], 0);
			close(1);
			dup2(fd[1], 1);
		}
		else if (pipe_in)
		{
			close(fd[1]);
			close(0);
			dup2(fd[0], 0);
		}
		else if (pipe_out)
		{
			close(fd[0]);
			close(1);
			dup2(fd[1], 1);
		}
		close(fd[0]);
		close(fd[1]);
		execvp(args[0], args);
		perror("");
		exit(EXIT_FAILURE);
	}
}

int main()
{
	int i, j;
	int fd[2];
	char *args[100];

	while(1)		// Start infinite loop
	{
		printf("\nEnter name of executable program with arguments (eg. ls ..): ");
		char exec[100];
		gets(exec);

		int len = strlen(exec);

		if (strcmp(exec,"quit")==0)	// quit command
			return 0;

		if (pipe(fd) < 0)
		{
			perror("Pipe could not be created ");
			exit(EXIT_FAILURE);
		}
		
		execute(exec, len, fd, 0);
		printf("\n");
	}
}