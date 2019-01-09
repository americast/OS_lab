#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main()
{
	while(1)
	{
		printf("\nEnter name of executable program: ");
		char exec[100];
		gets(exec);

		if (strcmp(exec,"quit")==0)
			return 0;

		pid_t p; 

		p = fork(); 
		
		if (p < 0)
		{ 
		    fprintf(stderr, "Fork Failed" ); 
		    // return 1; 
		} 
		else if (p == 0)
		{
			char *args[]={exec,NULL};
			execvp(args[0],NULL); 
		}
		wait(NULL);
	}
}