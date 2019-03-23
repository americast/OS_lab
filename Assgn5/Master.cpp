
#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <signal.h>
#include <semaphore.h>
#include <sys/msg.h>
using namespace std;

struct page_entry{
	int page;
	int frame;
	short int validity = -1;
	int use = 0;
};

pid_t sched_pid, mmu_pid;

void catcher(int signum)
{
    // if (signum == SIGUSR1)
    // {
    //     sigset_t myset;
    //     sigemptyset(&myset);

    //     printf("Suspending thread %lu\n", pthread_self());

    //     sigsuspend(&myset);
    //     printf("Thread %lu was sleeping\n", pthread_self());
    // }
    // else
    // {
    //     printf("Waking thread %lu\n", pthread_self());
    //     return;
    // }
    kill(sched_pid, SIGKILL);
    kill(mmu_pid, SIGKILL);
    cout<<"MASTER TERMINATES";
    exit(EXIT_SUCCESS);
}


int main()
{
	signal(SIGUSR1, catcher);
	int k, m, f;

	cout<<"Enter the number of processes : ";
	cin>>k;
	cout<<"Enter the number maximum number of pages required per process : ";
	cin>>m;
	cout<<"Enter the total number of main frames in memory : ";
	cin>>f;

	pid_t process_pid[k];

	key_t key_1 = ftok("SM1",65); 
	int shmid_1 = shmget(key_1, k*m*sizeof(page_entry), 0666|IPC_CREAT); 
	  
	key_t key_2 = ftok("SM2",65); 
	int shmid_2 = shmget(key_2, sizeof(int) + f * sizeof(int), 0666|IPC_CREAT); 

    key_t key_3 = ftok("MQ1", 65);
    int msgid_1 = msgget(key_3, 0666 | IPC_CREAT);


    key_t key_4 = ftok("MQ2", 65);
    int msgid_2 = msgget(key_4, 0666 | IPC_CREAT);

    key_t key_5 = ftok("MQ3", 65);
    int msgid_3 = msgget(key_5, 0666 | IPC_CREAT);

	char key_1_str[100], key_2_str[100], key_3_str[100], key_4_str[100], key_5_str[100];
	sprintf(key_1_str,"%d", key_1);
	sprintf(key_2_str,"%d", key_2);
	sprintf(key_3_str,"%d", key_3);
	sprintf(key_4_str,"%d", key_4);
	sprintf(key_5_str,"%d", key_5);

	sched_pid = fork();
	if(sched_pid == 0)
	{
        execlp("./scheduler", "./scheduler", key_3_str, key_4_str, (char *) NULL);
        printf("Failed to start scheduler \n");
        exit(EXIT_FAILURE);
    }
    printf("IN MASTER 1\n");

    mmu_pid = fork();
    if(mmu_pid == 0)
    {
    	execlp("./mmu", "./mmu", key_4_str, key_5_str, key_1_str, key_2_str, (char *) NULL);
    	printf("Failed to start mmu \n");
    	exit(EXIT_FAILURE);
    }
    printf("IN MASTER 2\n");

    for( int i =0; i<k; i++)
    {
	    process_pid[i] = fork();
	    if(process_pid[i] == 0)
	    {
	    	execlp("./process", "./process", key_3_str, key_5_str, (char *) NULL);
	    	printf("Failed to start process \n");
	    	exit(EXIT_FAILURE);
	    }
	    usleep(250000);
	}
    printf("IN MASTER 3\n");

    while(1)
    {
    	sleep(1);
    }


}