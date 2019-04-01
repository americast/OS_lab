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
	shmdt(fm);
	shmdt(pg);
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
			cout<<"Assigned page\n";
			pg[i * m + g].validity = 1;
			pg[i * m + g].page = frame_no;
			pg[i * m + g].frame = found_pos;
			break;
		}
	}

	
	// int pid;	// Need to know how to get this
	// msgsnd(key_MQ_2, &pid, sizeof(int), 0); 
	
	shmdt(fm);
	shmdt(pg);
	return found;
}



int checkPT(int page_no, int i, int m, int SM_1, int &new_frame_no)
{
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0); 
	int found = 0;
	for (int j = 0; j < m; j++)
	{
		if (pg[i*m + j].page == page_no && pg[i*m + j].validity)
		{
			new_frame_no = pg[i*m + j].frame;
			found = 1;
			TLB[page_no % s].memory_loc = new_frame_no;
			TLB[page_no % s].page_no = page_no;
			break;
		}
	}
	shmdt(pg);

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


	int key_MQ_2 = atoi(argv[1]);
	int key_MQ_3 = atoi(argv[2]);
	int key_SM_1 = atoi(argv[3]);
	int key_SM_2 = atoi(argv[4]);
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
	SM_1 = shmget(key_SM_1, (k + 1) * m * sizeof(page_entry), 0666|IPC_CREAT); 
	SM_2 = shmget(key_SM_2, f * sizeof(main_mem_frame), 0666|IPC_CREAT); 

	// page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0); 

	// for (int i = 0; i < k + 1; i++)
	// 	for (int g = 0; g < m; g++)
	// 	{
	// 		cout<<g<<endl;
	// 		pg[i * m + g].validity = -1;
	// 	}

	cout<<"Here -3\n";
	if (fork() == 0)
	{
		LRU_update(f);
	}

	cout<<"Here -4\n";
	pg_num pg_num_here;
	int page_num;
	while(1)
	{
		cout<<"MQ_3 is "<<key_MQ_3<<endl;
		msgrcv(MQ_3, &pg_num_here, sizeof(pg_num_here), 1, 0);

		page_num = atoi(pg_num_here.txt);

		int id = page_num % m;

		int pg_num_act = page_num / m;

		if (pg_num_act < 0)
			pg_num_act--;


		cout<<"Page num act: "<<pg_num_act<<endl;
		cout<<"Page ID: "<<id<<endl;


		cout<<"Here 0\n";
		if(pg_num_act == -9)
		{
			update_ff(id, m);
			strcpy(message.msg, "TERMINATED");
			cout<<"TERMINATE sent\n";
			message.type = 2;
			if (msgsnd(MQ_2, &message, sizeof(message), 0) < 0)
				perror("Terminate sending failed");
			continue;
		}
		int frame_num = -1;
		cout<<"Here 1\n";
		if (checkTLB(page_num, frame_num))
		{
			main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
			fm[frame_num].use = 1;
			shmdt(fm);
		}
		cout<<"Here 2\n";
		if (checkPT(page_num, id, m, SM_1, frame_num))
		{
			main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
			fm[frame_num].use++;
			shmdt(fm);
		}
		sprintf(pg_num_here.txt, "%d", frame_num);
		pg_num_here.type = 4;
		if (msgsnd(MQ_3, &pg_num_here, strlen(pg_num_here.txt), 0) < 0)
			perror("Error in sending frame num");


		if (frame_num >= 0)
			continue;

		cout<<"Here 3\n";
		handlePageFault(page_num, id, m, s, f, SM_1, SM_2, key_MQ_2);
		cout<<"Sending page fault handled\n";
		strcpy(message.msg, "PAGE FAULT HANDLED");
		message.type = 2;
		if (msgsnd(MQ_2, &message, sizeof(message), 0) < 0)
			perror("Page fault handled sending failed");
	}

}
