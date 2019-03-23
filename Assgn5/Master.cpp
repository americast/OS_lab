#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <signal.h>
#include <semaphore.h>
using namespace std;

typedef struct {
	int page;
	int frame;
	short int validity;
}page_entry;

// typedef * page_entry page_table;
int main()
{
	int k, m, f;
	// sem_t *semaphore = sem_open("FLAG_END", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
	sem_t *semaphore;
	sem_init(semaphore,777,0);

	cout<<"Enter the number of processes : ";
	cin>>k;
	cout<<"Enter the number maximum number of pages required per process : ";
	cin>>m;
	cout<<"Enter the total number of main frames in memory : ";
	cin>>f;
	pid_t sched_pid, mmu_pid, process_pid[k];
	int req_pages[k];

	// process_pid = (pid_t *)malloc(sizeof(pid_t)*k);

	// key_t key = ftok("shmfile",65); 

	// int shmid = shmget(key,k*m*sizeof(page_entry),0666|IPC_CREAT); 
	  
    // // shmat to attach to shared memory 
    // char *str = (char*) shmat(shmid,(void*)0,0); 
  
    // cout<<"Write Data : "; 
    // gets(str); 
  
    // printf("Data written in memory: %s\n",str); 
      
    // //detach from shared memory  
    // shmdt(str); 

	// page_entry **page_table = (page_entry **)malloc(sizeof(page_entry*)*k);
	// for(int i =0; i<k; i++)
	// {
	// 	page_table[i] = (page_entry *)malloc(sizeof(page_entry)*m);
	// 	for(int j=0; j<m; j++)
	// 	{
	// 		page_table[i][j].frame = -1;
	// 		page_table[i][j].validity = 0;
	// 	}
	// }

	sched_pid = fork();
	if(sched_pid == 0)
	{
        execlp("./scheduler", "./scheduler", (char *) NULL);
        printf("Failed to start scheduler \n");
        exit(EXIT_FAILURE);
    }
    printf("IN MASTER 1\n");

    mmu_pid = fork();
    if(mmu_pid == 0)
    {
    	execlp("./mmu", "./mmu", (char *) NULL);
    	printf("Failed to start mmu \n");
    	exit(EXIT_FAILURE);
    }
    printf("IN MASTER 2\n");

    for( int i =0; i<k; i++)
    {
	    process_pid[i] = fork();
	    if(process_pid[i] == 0)
	    {
	    	execlp("./process", "./process", (char *) NULL);
	    	printf("Failed to start process \n");
	    	exit(EXIT_FAILURE);
	    }
	    usleep(250000);
	}
    printf("IN MASTER 3\n");
    // printf("Val of semaphore %d\n", *semaphore);
    sem_wait(semaphore);
    // sem_destroy(semaphore);
    printf("Master ending\n");
    //delete others
    kill(sched_pid, SIGKILL);
    kill(mmu_pid, SIGKILL);
    exit(EXIT_SUCCESS);

}