#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <semaphore.h>
using namespace std;

int main()
{
	printf("HI I AM SCHED\n");
	sleep(5);
	printf("Scheduler giving signal\n");
	sem_t *semaphore = sem_open("FLAG_END", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
    sem_post(semaphore);

}