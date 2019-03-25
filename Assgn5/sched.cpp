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

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in servaddr, cliaddr; 

	memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(7433);
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed in sched"); 
        exit(EXIT_FAILURE); 
    }  
	
	while(1)
	{
		printf("INSIDE LOOP\n");
		int loop = 0;
		while(loop<20)
		{
			printf("Loop num :%d\n", loop);
		    socklen_t len = sizeof(cliaddr);
		    if (recvfrom(sockfd, &process, sizeof(process), MSG_DONTWAIT, 
					( struct sockaddr *) &cliaddr, &len) < 0)
			// if(msgrcv(rq_id, &process, sizeof(process), 1, 0)<0)
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
		printf("SCHED :: Process executing is : %d - %d \n", process.id, atoi(process.pid));
		kill(atoi(process.pid),SIGUSR2); // start process
		cout<<"Waiting for MMU\n";
		socklen_t len = sizeof(cliaddr);

		// msgrcv(mq_id, &message, sizeof(message), 1, 0);
		recvfrom(sockfd, &message, sizeof(message), 0, 
					(struct sockaddr *) &cliaddr, &len); 

		printf("SCHED :: Message is : %s\n", message.msg);
		if(strcmp(message.msg,"PAGE FAULT HANDLED")==0)
		{
    		servaddr.sin_port = htons(7433);
    		sendto(sockfd, &process, sizeof(process), 0, 
    					(const struct sockaddr *) &servaddr, sizeof(servaddr)); 

			// msgsnd(rq_id, &process, sizeof(process), 0);
		}

	}
	printf("SCHEDULER END\n");
	exit(EXIT_SUCCESS); 
}
