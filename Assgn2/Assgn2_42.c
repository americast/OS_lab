/***************************************************

                    ASSIGNMENT 2

    Group No.: 42

    Members:    Swastika Dutta  (16CS10060)
    			Sayan Sinha     (16CS10048)

****************************************************/

#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include<fcntl.h>

int execute(char *args[100], int fd[100][2], int i_here, int flag_pipe)
{

	char in_file[100], out_file[100];
	int i, flag_in = 0, flag_out =0, flag = 1, wait_flag = 0, f_in, f_out;

	for (i = 0; i < 100 && args[i]; i++)
	{
		if (strcmp(args[i], "<") == 0)			// To check input redirection
		{
			if(flag_out==1)
			{	
				fprintf(stderr, "Wrong command %s\n" );
				return(1);
			}
			flag_in = 1;
			flag = 0;
		}	
		else if(strcmp(args[i], ">") == 0)		// To check for output redirection
		{
			if(flag_in==1)
			{	
				fprintf(stderr, "Wrong command %s\n" );
				return(1);
			}
			flag_out = 1;
			flag = 0;
		}
		else if(flag_in==1)						// If input redirection is present
		{
			strcpy(in_file, args[i]);
			flag_in = 2;
			args[i] = NULL;
			args[i - 1] = NULL;
			printf("In file set\n");
		}
		else if(flag_out==1)					// If output redirection is present
		{
			strcpy(out_file, args[i]);
			flag_out = 2;
			args[i] = NULL;
			args[i - 1] = NULL;
			printf("out file set\n");
		}
		else
		{
			if(strcmp(args[i],"&")==0)			// For running in background
			{
				args[i] = NULL;
				wait_flag = 1;
			}
			if(!flag)
			{
				args[i] = NULL;
			}
		}
	}

	pid_t p = fork(); 	// spawning
	
	if (p < 0)
	{ 
	    fprintf(stderr, "Fork Failed" ); 
	} 
	else if (p == 0)
	{
		if(flag_in)								// If input redirection is found
		{
			f_in = open(in_file, O_RDONLY);
			if(f_in<0)
			{
				perror("Input file error ");
				exit(EXIT_FAILURE);
			}
			printf("Redirecting input\n");
			dup2(f_in,0);

		}
		if(flag_out)							// If output redirection is found
		{
			f_out = open(out_file, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if(f_out<0)
			{
				perror("Output file error ");
				exit(EXIT_FAILURE);
			}
			printf("Redirecting output\n");
			dup2(f_out,1);
		}
		if(flag_pipe==1)						// If pipe out is needed
		{
			close(fd[i_here][0]);
			close(1);
			dup(fd[i_here][1]);
			close(fd[i_here][1]);
		}
		else if (flag_pipe == 2)				// If both pipe in and pipe out are needed
		{
			close(1);
			dup(fd[i_here][1]);
			close(0);
			dup(fd[i_here-1][0]);
			close(fd[i_here-1][0]);
			close(fd[i_here][1]);
		}
		else if (flag_pipe == 3)				// If pipe out is needed
		{
			close(fd[i_here-1][1]);
			close(0);
			dup(fd[i_here-1][0]);
			close(fd[i_here-1][0]);
		}
		execvp(args[0],args); 					// replace the child process with the given process
		perror("There was an error");
		close(f_in);
		close(f_out);
		exit(EXIT_FAILURE);
	}

	if (flag_pipe == 1 || flag_pipe == 2)		// Close necessary pipes in parent to mark end of output
		close(fd[i_here][1]);
	if (flag_pipe == 2 || flag_pipe == 3)
		close(fd[i_here-1][0]);
	return wait_flag;

}

int main()
{
	int i, j, g_count = 0;
	char *args[100];

	while(1)		// Start infinite loop
	{
		printf("\n\nEnter name of executable program with arguments (eg. ls ..): ");
		char exec[100], ch;
		gets(exec);
		int fd[100][2];

		int len = strlen(exec);

		if (strcmp(exec,"quit")==0)	// quit command
			return 0;

		int flag_pipe=0; 
		
		int count = 0, actual_i = 0, pipe_count = 0;
		for (i = 0; i < 100; i++, actual_i++)
		{
			char here[100];
			sscanf(exec+count, "%s", here);
			if(strcmp(here,"|")==0)							// If pipe found, segregate the commands
			{
				if (pipe(fd[pipe_count]) < 0)
				{
					perror("Pipe could not be created ");
					exit(EXIT_FAILURE);
				}
				if (flag_pipe == 1 ||  flag_pipe == 2)		// If pipe out was in previous command
					flag_pipe = 2;
				if (flag_pipe == 0)
					flag_pipe = 1;
				args[actual_i] = NULL;
			}
			if(strcmp(here,"|"))							// If no pipe found till now
			{
				args[actual_i] = (char *) malloc(strlen(here)+1);
				strcpy(args[actual_i], here);
			}
			count+=strlen(here);
			if (count>=len || (flag_pipe && strcmp(here, "|")==0))	// If breaking condition is found
			{
				args[++actual_i] = NULL;
				int wait_flag;
				if (count>=len && flag_pipe)						// If all commands have been parsed and there was a pipe out previously
					wait_flag = execute(args, fd, pipe_count, 3);
				else if (flag_pipe)
					wait_flag = execute(args, fd, pipe_count, flag_pipe);	// If pipe in is needed
				else
					wait_flag = execute(args, fd, pipe_count, 0);			// If no pipes are involved
				dup2(stdin,0);
				dup2(stdout,1);
				dup2(stderr,2);
				int status, wpid; 
				if(!wait_flag)
				{
					while ((wpid = wait(&status)) > 0)						// Wait for all children to complete
					{
						if (status < 0)
							printf("Some error occured\n");
					}
				}
				for (j = 0; j < actual_i; j++)
					free(args[j]);
				actual_i = -1;
				pipe_count++;
				if (!flag_pipe || count>=len)
					break;
			}
			for (;*(exec+count)==' ';)						// Cross spaces to reach next command
				count++;
			if (count>=len)
				break;
		}
	}
}