#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <semaphore.h>
#include <sys/msg.h> 
using namespace std;

typedef struct rq { 
	short int id;
    pid_t pid; 
} ready_queue; 

typedef struct mq {
	char msg[20];
} message_queue;

int main(int argc, char **argv)
{
	key_t rq_t = *((key_t*)argv[0]);
	key_t mq_t = *((key_t*)argv[1]);
	printf("SCHEDULER INITIATED \n");

	ready_queue process;
	message_queue message;

	// printf("SCHEDULER INITIATED \n");
	
	int rq_id = msgget(rq_t, 0666 | IPC_CREAT); 
	int mq_id = msgget(mq_t, 0666 | IPC_CREAT);

	// printf("SCHEDULER INITIATED \n");
	
	while(1)
	{
		printf("INSIDE LOOP\n");
		int loop = 0;
		while(loop<10)
		{
			printf("Loop num :%d\n", loop);
			if(msgrcv(rq_id, &process, sizeof(process), 1, IPC_NOWAIT)<0)
			// if (errno == ENOMSG)
			{
				usleep(250000);
				loop++;
			}
			else
			{
				break;
			}
		}
		if(loop == 10)
		{
			pid_t master = getppid();
			kill(master,SIGUSR1);
			printf("No more process left. Signalling Master!\n");
			break;
		}
		printf("SCHED :: Process executing is : %d - %d \n", process.id, process.pid);
		kill(process.pid,SIGUSR1);
		msgrcv(mq_id, &message, sizeof(message), 1, 0);
		printf("SCHED :: Message is : %s\n", message.msg);
		if(strcmp(message.msg,"PAGE FAULT HANDLED")==0)
		{
			msgsnd(rq_id, &process, sizeof(process), 0);
		}

	}
	printf("SCHEDULER END\n");
	exit(EXIT_SUCCESS); 
}