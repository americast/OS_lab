#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include<fcntl.h>

int execute(char *args[100])
{

	char in_file[100], out_file[100];
	int i, flag_in = 0, flag_out =0, flag = 1, wait_flag = 0, p_start = 0, p_end =0, p = 0;
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

	p = fork(); 	// spawning
	
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
		execvp(args[0],args); 		// replace the child process with an example process
		perror("There was an error");
		exit(EXIT_FAILURE);
	}
	
	return wait_flag;

}

int main()
{
	int i;
	char *args[100];
	while(1)		// Start infinite loop
	{
		printf("\nEnter name of executable program with arguments (eg. ls ..): ");
		char exec[100];
		gets(exec);

		int len = strlen(exec);

		if (strcmp(exec,"quit")==0)	// quit command
			return 0;

		pid_t p; 
		
		// for (i = 0; i < 100; i++)
		// 	memset(args[i], 0, 100);
		// printf("Loop starts\n");
		int count = 0;
		for (i = 0; i < 100; i++)
		{
			char here[100];
			sscanf(exec+count, "%s", here);
			// free(args[i]);
			args[i] = (char *) malloc(strlen(here)+1);
			strcpy(args[i], here);
		
			// printf("Org now: %s\n", exec+count);
			count+=strlen(args[i]);
			if (count>=len)
				break;
			for (;*(exec+count)==' ';)
				count++;
		}
		i++;
		for (; i < 100; i++)
			args[i] = NULL;

		int wait_flag = execute(args);
		if(!wait_flag) wait(NULL);
		for (i = 0; i < 100; i++)
			free(args[i]);
	}
}