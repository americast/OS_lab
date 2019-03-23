#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
using namespace std;

void handlePageFault(int frame_no)
{
	
}

int main()
{
	printf("HI I AM MMU\n");
	sleep(100);

	int	key = ftok("progfile", 65); // key to be recvd from args
	int msgid = msgget(key, 0666 | IPC_CREAT);
	int frame_no;
	msgrcv(msgid, &frame_no, sizeof(int), 1, 0);
	int page_no;
	if (checkTLB(frame_no, page_no))
		return page_no;
	if (checkPT(frame_no, page_no))
		return page_no;

	if (fork() == 0)
	{
		handlePageFault(int frame_no);
	}
	return -1;

}