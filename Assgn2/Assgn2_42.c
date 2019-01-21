#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include<fcntl.h>

int execute(char *args[100], int fd[2], int flag_pipe)
{
	printf("arg: %s and flag: %d\n", args[0], flag_pipe);

	char in_file[100], out_file[100];
	int i, flag_in = 0, flag_out =0, flag = 1, wait_flag = 0;

	for (i = 0; i < 100 && args[i]; i++)
	{
		// printf("i is: %d\n", i);
		if (strcmp(args[i], "<") == 0)
		{
			if(flag_out==1)
			{	fprintf(stderr, "Wrong command %s\n" );
				return(1);
			}
			flag_in = 1;
			flag = 0;
		}	
		else if(strcmp(args[i], ">") == 0)
		{
			if(flag_in==1)
			{	fprintf(stderr, "Wrong command %s\n" );
				return(1);
			}
			flag_out = 1;
			flag = 0;
		}
		else if(flag_in==1)
		{
			strcpy(in_file, args[i]);
			flag_in = 2;
		}
		else if(flag_out==1)
		{
			strcpy(out_file, args[i]);
			flag_out = 2;
		}
		else
		{
			if(strcmp(args[i],"&")==0)
			{
				wait_flag = 1;
			}
			if(!flag)
			{
				args[i] = NULL;
			}
		}
		// printf("Command %d: %s\n", i, args[i]);
	}

	pid_t p = fork(); 	// spawning
	
	if (p < 0)
	{ 
	    fprintf(stderr, "Fork Failed" ); 
	    // return 1; 
	} 
	else if (p == 0)
	{
		if(flag_in)
		{
			int f_in = open(in_file, O_RDONLY);
			if(f_in<0)
			{
				perror("Input file error ");
				exit(EXIT_FAILURE);
			}
			dup2(f_in,0);
		}
		if(flag_out)
		{
			int f_out = open(out_file, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if(f_out<0)
			{
				perror("Output file error ");
				exit(EXIT_FAILURE);
			}
			dup2(f_out,1);
		}
		// close(fd[0]);
		if(flag_pipe==1)
		{
			close(fd[0]);
			dup2(fd[1],1);
			close(fd[1]);
		}
		else if (flag_pipe == 2)
		{
			dup2(fd[1], 1);
			dup2(fd[0], 0);
			close(fd[0]);
		}
		else if (flag_pipe == 3)
		{
			close(fd[1]);
			dup2(fd[0], 0);
			close(fd[0]);
		}
		execvp(args[0],args); 		// replace the child process with an example process
		perror("There was an error");
		exit(0);
	}

	return wait_flag;

}

int main()
{
	int i, j;
	int fd[2];
	char *args[100];

	if (pipe(fd) < 0)
	{
		perror("Pipe could not be created ");
		exit(EXIT_FAILURE);
	}
	while(1)		// Start infinite loop
	{
		printf("\nEnter name of executable program with arguments (eg. ls ..): ");
		char exec[100];
		gets(exec);

		int len = strlen(exec);

		if (strcmp(exec,"quit")==0)	// quit command
			return 0;

		int flag_pipe=0; 
		
		int count = 0, actual_i = 0;
		for (i = 0; i < 100; i++, actual_i++)
		{
			// printf("actual_i: %d\n", actual_i);
			dup2(stdin,0);
			dup2(stdout,1);
			dup2(stderr,2);

			char here[100];

			sscanf(exec+count, "%s", here);
			// free(args[i]);
			printf("%s\n",here);
			if(strcmp(here,"|")==0)
			{
				printf("Here1\n");
				if (flag_pipe == 1 ||  flag_pipe == 2)
					flag_pipe = 2;
				if (flag_pipe == 0)
					flag_pipe = 1;
				args[actual_i] = NULL;
			}
			if(strcmp(here,"|"))
			{
				printf("Here2\n");
				// printf("here: %s\n", here);
				args[actual_i] = (char *) malloc(strlen(here)+1);
				strcpy(args[actual_i], here);
			}
			// printf("Org now: %s\n", exec+count);
			count+=strlen(here);
			if (count>=len || (flag_pipe && strcmp(here, "|")==0))
			{
				printf("Here3\n");
				if (!flag_pipe)
					args[++actual_i] = NULL;
				int wait_flag;
				int bon;

				for (bon = 0; bon < 2; bon++)
					printf("bon is: %s\n", args[bon]);
				printf("flag is %d\n\n",flag_pipe);
				// printf("Sending: %s\n", args[0]);
				if (count>=len && flag_pipe)
					wait_flag = execute(args, fd, 3);
				else if (flag_pipe)
					wait_flag = execute(args, fd, flag_pipe);
				else
					wait_flag = execute(args, fd, 0);
				printf("Here4\n");
				if(!wait_flag) wait(NULL);
				printf("Here5\n");
				for (j = 0; j < i; j++)
					free(args[j]);
				actual_i = -1;
				if (!flag_pipe || count>=len)
					break;
			}
			for (;*(exec+count)==' ';)
				count++;
		}
	}
}