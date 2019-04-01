#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/msg.h>
#include <time.h>
#include "headers.h"
using namespace std;

rq ready_queue; 

int global_num = 0;

void catcher(int signum)
{
	if (signum == SIGUSR1)
	{
	    sigset_t myset;
	    sigemptyset(&myset);

	    printf("Suspending process %lu\n", getpid());

	    sigsuspend(&myset);
	    printf("Process %lu was sleeping\n", getpid());
	}
	else
	{
	    printf("Waking process %lu\n", getpid());
	    return;
	}
}

int main(int argc, char **argv)
{
	signal(SIGUSR1, catcher);
	signal(SIGUSR2, catcher);

	key_t rq_t = atoi(argv[1]);
	key_t pg_t = atoi(argv[2]);
	int page_ref_string_len = atoi(argv[3]);
	int total_pages = atoi(argv[4]);
	int m = total_pages;
	int id = atoi(argv[5]);

	printf("PROCESS INITIATED \n");
	cout<<"RQ_T "<<rq_t<<endl;

	rq process;
	
	int rq_id = msgget(rq_t, 0666 | IPC_CREAT); 
	int pg_id = msgget(pg_t, 0666 | IPC_CREAT);

	// sprintf(process.pid, "%d", getpid());
	process.type = 100 + id;
	cout<<"ID is: "<<id<<endl;
	memcpy(process.pid, &id, sizeof(int));
	pid_t pid_here = getpid();
	memcpy(process.pid + sizeof(int), &pid_here, sizeof(pid_t));
	cout<<"PID sent "<<process.pid<<endl;
	if (msgsnd(rq_id, &process, sizeof(int) + sizeof(pid_t), 0) < 0)
		perror("Msg sending failed");
	// signal(SIGUSR1, catcher);
	// sleep(1);
	kill(getpid(),SIGUSR1);
	srand(time(NULL));
	
	int count = 0;
	while(count<page_ref_string_len)
	{
		printf("INSIDE LOOP\n");
		int page_num = (rand()%m)*m+id;
		int frame_num;
		cout<<"Page num sent "<<page_num<<endl;
		pg_num here;
		here.type = 1;
		sprintf(here.txt, "%d", page_num);
		if (msgsnd(pg_id, &here, strlen(here.txt) + 1, 0) < 0)
			perror("Page num sending error");
		msgrcv(pg_id, &here, sizeof(here), 4, 0);
		frame_num = atoi(here.txt);
		cout<<"Frame num received "<<frame_num<<endl;
		if(frame_num<0)
		{
			if(frame_num == -1)
			{
				kill(getpid(), SIGUSR1);
			}
			else
			{
				cout<<"Invalid memory access, terminating\n";
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			count ++;
		}
	}
	int reply = -9*m + id;
	pg_num here;
	here.type = 1;
	sprintf(here.txt, "%d", reply);
	if (msgsnd(pg_id, &here, strlen(here.txt), 0) < 0)
		perror("Sending last page id failed");
	printf("PROCESS END\n");
	exit(EXIT_SUCCESS); 

}
