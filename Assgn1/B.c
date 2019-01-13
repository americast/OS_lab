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
		printf("\nEnter name of executable program (eg. ls ..): ");
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

		// for (i = 0; i < 100; i++)
		// 	printf("Command %d: %s\n", i, args[i]);

		p = fork(); 	// spawning
		
		if (p < 0)
		{ 
		    fprintf(stderr, "Fork Failed" ); 
		    // return 1; 
		} 
		else if (p == 0)
		{
			execvp(args[0],args); 		// replace the child process with an example process
			return 0;
		}
			// printf("PID: %d\n", p);
		wait(NULL);
		for (i = 0; i < 100; i++)
			free(args[i]);
	}
}