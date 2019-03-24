#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h>  
#include <sys/msg.h> 
using namespace std;
int SM_1, SM_2, MQ_2, MQ_3;


struct page_entry{
	int page;
	int frame;
	int validity = -1;
};

struct main_mem_frame{
	int frame;
	int validity;
	int use = 0;
};

typedef struct mq {
	char msg[100];
} message_queue;

struct map_
{
	int page_no;
	int memory_loc;
};
int s;
map_ *TLB;
int count = 0;

void update_ff(int i, int m)
{

	main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0);

	for (int j = 0; j < m; j++)
	{
		if (pg[i * m + j].validity)
			fm[pg[i * m + j].frame].validity = -1;
	}
}

int handlePageFault(int frame_no, int i, int m, int s, int f, int SM_1, int SM_2, int key_MQ_2)
{
	main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
	page_entry *pg = (page_entry*) shmat(SM_1,(void*)0,0); 
	int found = 0;
	int min_use = INT_MAX;
	int min_use_pos = 0;
	int found_pos = 0;
	for (int j = 0; j < f; j++)
	{
		if (fm[j].validity == -1)
		{
			fm[j].frame = frame_no;
			fm[j].validity = 1;
			fm[j].use = 0;
			found = 1;
			found_pos = j;
			break;
		}
		else
		{
			if (fm[j].use < min_use)
			{
				min_use = fm[j].use;
				min_use_pos = j;
			}
		}

	}

	if (!found)
	{
		fm[min_use_pos].frame = frame_no;
		fm[min_use_pos].validity = 1;
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

int main(int argc, char* argv[])
{
	printf("HI I AM MMU\n");
	// sleep(100);
	kill(getpid(), SIGUSR1);

	message_queue message;

	int id, m, k, f; // Need to get these, also value of s


	int key_MQ_2 = atoi(argv[1]);
	int key_MQ_3 = atoi(argv[2]);
	int key_SM_1 = atoi(argv[3]);
	int key_SM_2 = atoi(argv[4]);
	s = atoi(argv[5]);
	m = atoi(argv[6]);
	k = atoi(argv[7]);
	f = atoi(argv[8]);
	for (int i = 0; i < s; i++)
	{
		TLB[i].page_no = -1;
		TLB[i].memory_loc = -1;
	}

	map_ TLB[s];

	MQ_2 = msgget(key_MQ_2, 0666 | IPC_CREAT);
	MQ_3 = msgget(key_MQ_3, 0666 | IPC_CREAT);

	SM_1 = shmget(key_SM_1, k * m * sizeof(page_entry), 0666|IPC_CREAT); 
	SM_2 = shmget(key_SM_2, f * sizeof(main_mem_frame), 0666|IPC_CREAT); 

	int page_num;
	while(1)
	{
		msgrcv(MQ_3, &page_num, sizeof(int), 1, 0);

		int id = page_num % m;

		int pg_num_act = page_num / m;


		if(pg_num_act == -9)
		{
			update_ff(id, m);
			strcpy(message.msg, "TERMINATED");
			msgsnd(MQ_2, &message, sizeof(message), 0);
			continue;
		}
		int frame_num = -1;
		main_mem_frame *fm = (main_mem_frame*) shmat(SM_2,(void*)0,0);
		if (checkTLB(page_num, frame_num))
		{
			fm[frame_num].use++;
			char fm_no_str[100];
			sprintf(fm_no_str, "%p", &fm[frame_num].frame);
			int fm_no = atoi(fm_no_str);
			msgsnd(MQ_3, &fm_no, sizeof(frame_num), 0); 
			continue;
		}
		if (checkPT(page_num, id, m, SM_1, frame_num))
		{
			fm[frame_num].use++;
			char fm_no_str[100];
			sprintf(fm_no_str, "%p", &fm[frame_num].frame);
			int fm_no = atoi(fm_no_str);
			msgsnd(MQ_3, &fm_no, sizeof(frame_num), 0); 
			continue;
		}
		msgsnd(MQ_3, &frame_num, sizeof(frame_num), 0); 

		handlePageFault(page_num, id, m, s, f, SM_1, SM_2, key_MQ_2);
		strcpy(message.msg, "PAGE FAULT HANDLED");
		msgsnd(MQ_2, &message, sizeof(message), 0); 
	}

}
