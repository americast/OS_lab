/***************************************************

					ASSIGNMENT 1 (b)

	Group No.: 42

	Members:	Sayan Sinha 	(16CS10048)
				Swastika Dutta	(16CS10060)

****************************************************/


#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

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
		
		int count = 0;

		// Input the arguments and name of executable program as array
		for (i = 0; i < 100; i++)
		{
			char here[100];
			sscanf(exec+count, "%s", here);
			args[i] = (char *) malloc(strlen(here)+1);
			strcpy(args[i], here);
		
			count+=strlen(args[i]);
			if (count>=len)
				break;
			for (;*(exec+count)==' ';)
				count++;
		}

		i++;
		for (; i < 100; i++)
			args[i] = NULL;

		p = fork(); 	// spawning
		
		if (p < 0)
		{ 
		    fprintf(stderr, "Fork Failed" ); 
		    return 0; 
		} 
		else if (p == 0)
		{
			execvp(args[0],args); 		// replace the child process with an example process
			perror("There was an error");
			return 0;
		}

		// To wait till termination of child process:
		wait(NULL);
		for (i = 0; i < 100; i++)
			free(args[i]);
	}
}