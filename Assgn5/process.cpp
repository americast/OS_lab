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

	sprintf(process.pid, "%d", getpid());
	process.id = id;


	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in servaddr, cliaddr; 

	memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(7763 + id);
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed in proc"); 
        exit(EXIT_FAILURE); 
    } 


	cout<<"PID sent "<<process.pid<<endl;

	servaddr.sin_port = htons(7433); 
	sendto(sockfd, &process, sizeof(process), 0, 
				(const struct sockaddr *) &servaddr, sizeof(servaddr)); 
	// msgsnd(rq_id, &process, sizeof(process), 0);
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
		here.type = page_num;
		servaddr.sin_port = htons(6680); 
		sendto(sockfd, &here, sizeof(here), 0, 
					(const struct sockaddr *) &servaddr, sizeof(servaddr)); 

		socklen_t len = sizeof(cliaddr);
		recvfrom(sockfd, &here, sizeof(here), 0, 
					(struct sockaddr *) &cliaddr, &len); 

		// msgsnd(pg_id, &here, sizeof(here), 0);
		// msgrcv(pg_id, &here, sizeof(here), 1, 0);
		frame_num = here.type;
		cout<<"Frame num received "<<frame_num<<endl;
		if(frame_num<0)
		{
			if(frame_num == -1)
			{
				kill(getpid(), SIGUSR1);
			}
			else
			{
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			count ++;
		}
	}
	int reply = -9*m + id;
	servaddr.sin_port = htons(6680); 
	sendto(sockfd, &reply, sizeof(reply), 0, 
					(const struct sockaddr *) &servaddr, sizeof(servaddr)); 

	// msgsnd(pg_id, &reply, sizeof(reply), 0);
	printf("SCHEDULER END\n");
	exit(EXIT_SUCCESS); 

}
