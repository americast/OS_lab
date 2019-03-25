#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h>  
#include <sys/msg.h> 
#include "headers.h"
using namespace std;
int SM_1, SM_2, MQ_2, MQ_3;

mq message_queue;

int s;
map_ *TLB;
int count = 0;

void update_ff(int i, int m)
{

	main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0);

	for (int j = 0; j < m; j++)
	{
		if (!pg[i * m + j].validity)
			fm[pg[i * m + j].frame].free = 1;
	}
}

int handlePageFault(int frame_no, int i, int m, int s, int f, int SM_1, int SM_2, int key_MQ_2)
{
	cout<<"Page fault occured\n";
	main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0); 
	int found = 0;
	int min_use = INT_MAX;
	int min_use_pos = 0;
	int found_pos = 0;
	for (int j = 0; j < f; j++)
	{
		if (fm[j].free == 1)
		{
			fm[j].frame = frame_no;
			fm[j].free = 0;
			fm[j].use = 0;
			found = 1;
			found_pos = j;
			break;
		}
		else
		{
			if (fm[j].hist < min_use)
			{
				min_use = fm[j].hist;
				min_use_pos = j;
			}
		}

	}

	if (!found)
	{
		fm[min_use_pos].frame = frame_no;
		fm[min_use_pos].free = 0;
		fm[min_use_pos].use = 0;
		found = 1;
		found_pos = min_use_pos;
	}



	// int lookup = frame_no % s;
	// TLB[lookup].frame_no == frame_no;
	// TLB[lookup].memory_loc = found_pos;

	for (int g = 0; g < m; g++)
	{
		if (pg[i * m + g].validity == -1)  // eta ektu dekhte hbe
		{
			pg[i * m + g].validity = 1;
			pg[i * m + g].page = frame_no;
			pg[i * m + g].frame = found_pos;
			break;
		}
	}

	
	int pid;	// Need to know how to get this
	msgsnd(key_MQ_2, &pid, sizeof(int), 0); 
	return found;
}



int checkPT(int page_no, int i, int m, int SM_1, int &new_frame_no)
{
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0); 
	int found = 0;
	for (int j = 0; j < m; j++)
	{
		if (pg[i*j*sizeof(page_entry)].page == page_no)
		{
			new_frame_no = pg[i*j*sizeof(page_entry)].frame;
			found = 1;
			TLB[page_no % s].memory_loc = new_frame_no;
			TLB[page_no % s].page_no = page_no;
			break;
		}
	}

	return found;
}

int checkTLB(int page_num, int &frame_num)
{
	int lookup = page_num % s;

	if (TLB[lookup].page_no == page_num)
	{
		frame_num = TLB[lookup].memory_loc;
		return 1;
	}
	return 0;
}

void LRU_update(int f)
{
	while(1)
	{
		usleep(50000);
		main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
		for (int i = 0; i < f; i++)
		{
			if (fm[f].free)
				continue;
			int MSB = fm[f].use << (sizeof(int) - 1);
			fm[f].hist = fm[f].hist >> 1;
			fm[f].hist += MSB;
		}
	}
}

int main(int argc, char* argv[])
{
	printf("HI I AM MMU\nand argc is: %d\n", argc);
	cout<<"Here -1\n";
	// sleep(100);
	// kill(getpid(), SIGUSR1);

	mq message;

	int id, m, k, f; // Need to get these, also value of s


	int key_MQ_2 = ftok("MQ2", 4);
	int key_MQ_3 = ftok("MQ3", 5);
	int key_SM_1 = ftok("SM1", 1);
	int key_SM_2 = ftok("SM2", 2);
	s = atoi(argv[5]);
	m = atoi(argv[6]);
	k = atoi(argv[7]);
	f = atoi(argv[8]);
	cout<<"Here -2 is "<<s<<"\n";

	TLB = (map_ *) malloc(sizeof(map_) * s);
	for (int i = 0; i < s; i++)
	{
		cout<<"Here--\n";
		TLB[i].page_no = -1;
		TLB[i].memory_loc = -1;
	}

	// map_ TLB[s];

	cout<<"Here -2.5\n";
	MQ_2 = msgget(key_MQ_2, 0666 | IPC_CREAT);
	MQ_3 = msgget(key_MQ_3, 0666 | IPC_CREAT);

	cout<<"Here -2.75\n";
	SM_1 = shmget(key_SM_1, k * m * sizeof(page_entry), 0666|IPC_CREAT); 
	SM_2 = shmget(key_SM_2, f * sizeof(main_mem_frame), 0666|IPC_CREAT); 


	cout<<"Here -3\n";
	if (fork() == 0)
	{
		LRU_update(f);
	}

	cout<<"Here -4\n";
	pg_num pg_num_here;
	int page_num;


	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in servaddr, cliaddr; 

	memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(6680);
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed in MMU"); 
        exit(EXIT_FAILURE); 
    } 
	while(1)
	{
		// cout<<"MQ_3 is "<<key_MQ_3<<endl;
		// msgrcv(MQ_3, &pg_num_here, sizeof(pg_num_here), 1, 0);
		socklen_t len = sizeof(cliaddr);
		recvfrom(sockfd, &pg_num_here, sizeof(pg_num_here), 0, 
					(struct sockaddr *) &cliaddr, &len); 

		page_num = pg_num_here.type;

		int id = page_num % m;

		int pg_num_act = page_num / m;


		cout<<"Here 0\n";
		if(pg_num_act == -9)
		{
			update_ff(id, m);
			strcpy(message.msg, "TERMINATED");
			cout<<"TERMINATE sent\n";
    		servaddr.sin_port = htons(7433);
			sendto(sockfd, &message, sizeof(message), 0, 
						(const struct sockaddr *) &servaddr, sizeof(servaddr)); 

			// msgsnd(MQ_2, &message, sizeof(message), 0);
			continue;
		}
		// servaddr.sin_port = htons(7763);
		int frame_num = -1;
		main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
		cout<<"Here 1\n";
		if (checkTLB(page_num, frame_num))
		{
			fm[frame_num].use = 1;
			char fm_no_str[100];
			sprintf(fm_no_str, "%p", &fm[frame_num].frame);
			int fm_no = atoi(fm_no_str);
			pg_num_here.type = fm_no;
			// msgsnd(MQ_3, &pg_num_here, sizeof(pg_num), 0); 
			sendto(sockfd, &pg_num_here, sizeof(pg_num_here), 0, 
				(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
	
			continue;
		}
		cout<<"Here 2\n";
		if (checkPT(page_num, id, m, SM_1, frame_num))
		{
			fm[frame_num].use++;
			char fm_no_str[100];
			sprintf(fm_no_str, "%p", &fm[frame_num].frame);
			int fm_no = atoi(fm_no_str);
			pg_num_here.type = fm_no;
			sendto(sockfd, &pg_num_here, sizeof(pg_num_here), 0, 
						(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
			
			// msgsnd(MQ_3, &pg_num_here, sizeof(pg_num), 0); 
			continue;
		}
		pg_num_here.type = frame_num;
		sendto(sockfd, &pg_num_here, sizeof(pg_num_here), 0, 
					(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
		
		// msgsnd(MQ_3, &pg_num_here, sizeof(pg_num), 0); 

		cout<<"Here 3\n";
		handlePageFault(page_num, id, m, s, f, SM_1, SM_2, key_MQ_2);
		cout<<"Sending page fault handled\n";
		servaddr.sin_port = htons(7433);
		sendto(sockfd, &message, sizeof(message), 0, 
					(const struct sockaddr *) &servaddr, sizeof(servaddr)); 

		strcpy(message.msg, "PAGE FAULT HANDLED");
		msgsnd(MQ_2, &message, sizeof(message), 0); 
	}

}
