#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include<fcntl.h>

int execute(char *args[100], int fd[100][2], int i_here, int flag_pipe)
{
	// printf("arg: %s and flag: %d\n", args[0], flag_pipe);

	char in_file[100], out_file[100];
	int i, flag_in = 0, flag_out =0, flag = 1, wait_flag = 0, f_in, f_out;

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
		// dup2(stdin,0);
		// dup2(stdout,1);
		// dup2(stderr,2);
		if(flag_in)
		{
			f_in = open(in_file, O_RDONLY);
			if(f_in<0)
			{
				perror("Input file error ");
				exit(EXIT_FAILURE);
			}
			dup2(f_in,0);

		}
		if(flag_out)
		{
			f_out = open(out_file, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if(f_out<0)
			{
				perror("Output file error ");
				exit(EXIT_FAILURE);
			}
			dup2(f_out,1);
		}
		// close(fd[0]);
		// printf("i here: %d\n", i_here);
		if(flag_pipe==1)
		{
			close(fd[i_here][0]);
			close(1);
			dup(fd[i_here][1]);
			close(fd[i_here][1]);
		}
		else if (flag_pipe == 2)
		{
			close(1);
			dup(fd[i_here][1]);
			close(0);
			dup(fd[i_here-1][0]);
			close(fd[i_here-1][0]);
			close(fd[i_here][1]);
		}
		else if (flag_pipe == 3)
		{
			close(fd[i_here-1][1]);
			close(0);
			dup(fd[i_here-1][0]);
			close(fd[i_here-1][0]);
		}
		execvp(args[0],args); 		// replace the child process with an example process
		perror("There was an error");
		close(f_in);
		close(f_out);
		if(flag_pipe==2) close(fd[1]);
		exit(EXIT_FAILURE);
	}

	if (flag_pipe == 1 || flag_pipe == 2)
		close(fd[i_here][1]);
	if (flag_pipe == 2 || flag_pipe == 3)
		close(fd[i_here-1][0]);
	return wait_flag;

}

int main()
{
	int i, j, g_count = 0;
	char *args[100];

	// if (pipe(fd) < 0)
	// {
	// 	perror("Pipe could not be created ");
	// 	exit(EXIT_FAILURE);
	// }
	while(1)		// Start infinite loop
	{
		printf("\nEnter name of executable program with arguments (eg. ls ..): ");
		// dup2(stdin,0);
		// dup2(stdout,1);
		// dup2(stderr,2);
		char exec[100], ch;
		// if (g_count)
		// 	while ((ch = getchar()) != '\n' && ch != EOF);
		// fflush(stdin);
		gets(exec);
		int fd[100][2];
		// g_count++;



		int len = strlen(exec);

		if (strcmp(exec,"quit")==0)	// quit command
			return 0;

		int flag_pipe=0; 
		
		int count = 0, actual_i = 0, pipe_count = 0;
		for (i = 0; i < 100; i++, actual_i++)
		{
			// printf("\n");
			// printf("actual_i: %d\n", actual_i);
			// dup2(stdin,0);
			// dup2(stdout,1);
			// dup2(stderr,2);

			char here[100];
			sscanf(exec+count, "%s", here);
			// free(args[i]);
			// printf("%s\n",here);
			if(strcmp(here,"|")==0)
			{
				if (pipe(fd[pipe_count]) < 0)
				{
					perror("Pipe could not be created ");
					exit(EXIT_FAILURE);
				}
				// printf("Here1\n");
				if (flag_pipe == 1 ||  flag_pipe == 2)
					flag_pipe = 2;
				if (flag_pipe == 0)
					flag_pipe = 1;
				args[actual_i] = NULL;
			}
			if(strcmp(here,"|"))
			{
				// printf("Here2\n");
				// printf("here: %s\n", here);
				args[actual_i] = (char *) malloc(strlen(here)+1);
				strcpy(args[actual_i], here);
			}
			// printf("Org now: %s\n", exec+count);
			count+=strlen(here);
			if (count>=len || (flag_pipe && strcmp(here, "|")==0))
			{
				// printf("Here3\n");
				// printf("flag pipe is: %d\n", flag_pipe);
				// printf("actual_i is: %d\n", actual_i);
				args[++actual_i] = NULL;
				int wait_flag;
				int bon;


				// for (bon = 0; bon < 3; bon++)
				// 	printf("bon is: %s\n", args[bon]);
				// printf("flag is %d\n\n",flag_pipe);
				// printf("Sending: %s\n", args[0]);
				if (count>=len && flag_pipe)
				{
					wait_flag = execute(args, fd, pipe_count, 3);
					// close(fd[pipe_count][0]);
				}
				else if (flag_pipe)
					wait_flag = execute(args, fd, pipe_count, flag_pipe);
				else
					wait_flag = execute(args, fd, pipe_count, 0);
				// printf("Here4\n");
				// close(fd[0]);
				// char eof_here[2];
				// sprintf(eof_here, "%d", EOF);
				// write(fd[1], eof_here, 2);
				// close(fd[0]);
				dup2(stdin,0);
				dup2(stdout,1);
				dup2(stderr,2);
				if(!wait_flag) wait(NULL);
				// sleep(1);
				// printf("Here5\n");
				for (j = 0; j < actual_i; j++)
					free(args[j]);
				actual_i = -1;
				pipe_count++;
				if (!flag_pipe || count>=len)
				{
					// execlp("./a.out", "./a.out");	
					// exit(0);
					break;
				}
			}
			for (;*(exec+count)==' ';)
				count++;
			if (count>=len)
				break;
		}
	}
}