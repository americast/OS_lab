#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h>  
#include <sys/msg.h> 
using namespace std;


struct page_entry{
	int page;
	int frame;
	short int validity;
	int use;
};

struct map_
{
	int frame_no;
	int memory_loc;
};
int s = 128;
map_ TLB[128];

int handlePageFault(int frame_no, int i, int m, int s, int SM_1, int SM_2, int key_MQ_2)
{
	int *fm = (int*) shmat(SM_2,(void*)0,0);
	int n = fm[0];
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0); 
	int found = 0;
	int min_use = INT_MAX;
	int min_use_pos = 0;
	for (int j = 0; j < m; j++)
	{
		if (pg[i*j*sizeof(page_entry)].validity == -1)
		{
			pg[i*j*sizeof(page_entry)].frame = frame_no;
			pg[i*j*sizeof(page_entry)].page = fm[n];
			pg[i*j*sizeof(page_entry)].validity = 1;
			pg[i*j*sizeof(page_entry)].use = 0;
			found = 1;
			fm[0]--;
			break;
		}
		else
		{
			if (pg[i*j*sizeof(page_entry)].use < min_use)
			{
				min_use = pg[i*j*sizeof(page_entry)].use;
				min_use_pos = j;
			}
		}

	}

	if (!found)
	{
		pg[i*min_use_pos*sizeof(page_entry)].frame = frame_no;
		pg[i*min_use_pos*sizeof(page_entry)].page = fm[n];
		pg[i*min_use_pos*sizeof(page_entry)].validity = 1;
		pg[i*min_use_pos*sizeof(page_entry)].use = 0;
		found = 1;
	}



	int lookup = frame_no % s;
	TLB[lookup].frame_no == frame_no;
	TLB[lookup].memory_loc = fm[n];

	
	int pid;	// Need to know how to get this
	msgsnd(key_MQ_2, &pid, sizeof(int), 0); 
	return found;
}



int checkPT(int frame_no, int i, int m, int SM_1, int &new_frame_no)
{
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0); 
	int found = 0;
	for (int j = 0; j < m; j++)
	{
		if (pg[i*j*sizeof(page_entry)].frame == frame_no)
		{
			new_frame_no = pg[i*j*sizeof(page_entry)].page;
			pg[i*j*sizeof(page_entry)].use++;
			found = 1;
			break;
		}
	}

	return found;
}

int checkTLB(int frame_no, int &new_frame_no)
{
	int lookup = frame_no % s;

	if (TLB[lookup].frame_no == frame_no)
	{
		new_frame_no = TLB[lookup].memory_loc;
		return 1;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	printf("HI I AM MMU\n");
	sleep(100);

	int i, m; // Need to get these

	for (int i = 0; i < s; i++)
	{
		TLB[i].frame_no = -1;
		TLB[i].memory_loc = -1;
	}

	int key_MQ_2 = atoi(argv[1]);
	int key_MQ_3 = atoi(argv[2]);
	int key_SM_1 = atoi(argv[3]);
	int key_SM_2 = atoi(argv[4]);

	int MQ_2 = msgget(key_MQ_2, 0666 | IPC_CREAT);
	int MQ_3 = msgget(key_MQ_3, 0666 | IPC_CREAT);

	int SM_1 = shmget(key_SM_1, 1024, 0666|IPC_CREAT); 
	int SM_2 = shmget(key_SM_2, 1024, 0666|IPC_CREAT); 

	int frame_no;
	while(1)
	{
		msgrcv(MQ_3, &frame_no, sizeof(int), 1, 0);
		int updated_frame_no = -1;
		if (checkTLB(frame_no, updated_frame_no))
		{
			msgsnd(MQ_3, &updated_frame_no, sizeof(updated_frame_no), 0); 
		}
		if (checkPT(frame_no, i, m, SM_1, updated_frame_no))
		{
			msgsnd(MQ_3, &updated_frame_no, sizeof(updated_frame_no), 0); 
			continue;
		}
		if (fork() == 0)
		{
			handlePageFault(frame_no, i, m, s, SM_1, SM_2, key_MQ_2);
		}
		msgsnd(MQ_3, &updated_frame_no, sizeof(updated_frame_no), 0); 
	}

}