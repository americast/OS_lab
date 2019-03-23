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

	cout<<"Enter the number of processes : ";
	cin>>k;
	cout<<"Enter the number maximum number of pages required per process : ";
	cin>>m;
	cout<<"Enter the total number of main frames in memory : ";
	cin>>f;

	key_t key_1 = ftok("SM1",65); 

	int shmid_1 = shmget(key_1, k*m*sizeof(page_entry), 0666|IPC_CREAT); 
	  


	key_t key_2 = ftok("SM2",65); 

	int shmid_2 = shmget(key_2, sizeof(int) + f * sizeof(int), 0666|IPC_CREAT); 

    // // shmat to attach to shared memory 
    // char *str = (char*) shmat(shmid,(void*)0,0); 
  
    // cout<<"Write Data : "; 
    // gets(str); 
  
    // printf("Data written in memory: %s\n",str); 
      
    // //detach from shared memory  
    // shmdt(str); 

    key_t key_3 = ftok("MQ1", 65);
    int msgid_1 = msgget(key_3, 0666 | IPC_CREAT);


    key_t key_4 = ftok("MQ2", 65);
    int msgid_2 = msgget(key_4, 0666 | IPC_CREAT);



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

	char key_3_str[100], key_4_str[100];

	sprintf(key_3_str,"%d", key_3);
	sprintf(key_4_str,"%d", key_4);

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
    // sem_wait(semaphore);
    // sem_destroy(semaphore);
    printf("Master ending\n");
    //delete others
    kill(sched_pid, SIGKILL);
    kill(mmu_pid, SIGKILL);
    exit(EXIT_SUCCESS);

}