#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/msg.h>
using namespace std;

typedef struct rq { 
	short int id;
    pid_t pid; 
} ready_queue; 

void catcher(int signum)
{
    if (signum == SIGUSR1)
    {
    	kill(getpid(), SIGSTOP);
    }
    else
    {
 		kill(getpid(), SIGCONT);
    }
}

int main(int argc, char **argv)
{
	signal(SIGUSR1, catcher);
	signal(SIGUSR2, catcher);

	key_t rq_t = *((key_t*)argv[1]);
	key_t pg_t = *((key_t*)argv[2]);
	int total_pages = *((int*)argv[3]);
	int page_ref_string_len = *((int*)argv[3]);
	printf("PROCESS INITIATED \n");

	ready_queue process;
	
	int rq_id = msgget(rq_t, 0666 | IPC_CREAT); 
	int pg_id = msgget(pg_t, 0666 | IPC_CREAT);

	process.pid = getpid();

	msgsnd(rq_id, &process, sizeof(process), 0);
	// signal(SIGUSR1, catcher);
	kill(getpid(),SIGUSR1);
	
	int count = 0;
	while(count<page_ref_string_len)
	{
		printf("INSIDE LOOP\n");
		int page_num = rand()%total_pages;
		int frame_num;
		msgsnd(pg_id, &page_num, sizeof(int), 0);
		msgrcv(pg_id, &frame_num, sizeof(int), 1, 0);

		if(frame_num<0)
		{
			if(frame_num == -1)
			{
				// count --;
				kill(getpid(), SIGUSR1);
			}
			else
			{
				exit(EXIT_SUCCESS);
			}
		}
		else
		{
			count ++;
		}
	}
	int reply = -9;
	msgsnd(pg_id, &reply, sizeof(reply), 0);
	printf("SCHEDULER END\n");
	exit(EXIT_SUCCESS); 

}