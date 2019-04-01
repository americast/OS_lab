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
	int k = atoi(argv[3]);
	int max_process = k;
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
		int ids_done[100 + max_process + 1] = {0};
		while(loop<20)
		{
			printf("Loop num :%d\n", loop);
			int msg_rcvd_flag = 0;
			for (int p = 100 + max_process; p >= 100; p--)
			{
				cout<<"id is "<<p<<endl;
				if (ids_done[p])
				{
					cout<<"id done: "<<p<<endl;
					cout<<"done value: "<<ids_done[p]<<endl;
					continue;
				}
				if(msgrcv(rq_id, &process, sizeof(process), p, IPC_NOWAIT)<0)
				{
					usleep(250000);
				}
				else
				{
					msg_rcvd_flag = 1;
					break;
				}
			}
			if (msg_rcvd_flag == 0)
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
		int id; pid_t pid;
		memcpy(&id, process.pid, sizeof(int));
		memcpy(&pid, process.pid + sizeof(int), sizeof(pid_t));
		printf("SCHED :: Process executing is : %d - %d \n", id, pid);
		kill(pid,SIGUSR2); // start process
		cout<<"Waiting for MMU\n";
		msgrcv(mq_id, &message, sizeof(message), 2, 0);
		printf("SCHED :: Message is : %s\n", message.msg);
		if(strcmp(message.msg,"PAGE FAULT HANDLED")==0)
		{
			process.type = 100;
			msgsnd(rq_id, &process, sizeof(int) + sizeof(pid_t), 0);
		}
		else
		{
			ids_done[100 + id] = 1;
			cout<<"\nDone with "<<id<<endl;
		}

	}
	printf("SCHEDULER END\n");
	exit(EXIT_SUCCESS); 
}
