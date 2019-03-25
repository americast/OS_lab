#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <semaphore.h>
#include <sys/msg.h> 
#include "headers.h"
using namespace std;

rq ready_queue; 

mq message_queue;

int main(int argc, char **argv)
{
	key_t rq_t = atoi(argv[1]);
	key_t mq_t = atoi(argv[2]);
	printf("SCHEDULER INITIATED \n");
	cout<<"RQ_T "<<rq_t<<endl;

	rq process;
	mq message;
	
	int rq_id = msgget(rq_t, 0666 | IPC_CREAT); 
	int mq_id = msgget(mq_t, 0666 | IPC_CREAT);
	
	while(1)
	{
		printf("INSIDE LOOP\n");
		int loop = 0;
		while(loop<20)
		{
			printf("Loop num :%d\n", loop);
			if(msgrcv(rq_id, &process, sizeof(process), 1, 0)<0)
			{
				usleep(250000);
				loop++;
			}
			else
			{
				break;
			}
		}
		if(loop == 20)
		{
			printf("No more process left. Signalling Master!\n");
			kill(getppid(),SIGUSR1);
			break;
		}
		printf("SCHED :: Process executing is : %d - %d \n", process.id, process.pid);
		kill(atoi(process.pid),SIGUSR2); // start process
		cout<<"Waiting for MMU\n";
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
