#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

#define M 80
#define QUANTUM 1
#define N 10

int BUFFER[M];
int pos = 0;
pthread_t P_threads[N];
pthread_t C_threads[N];
vector<pthread_t> ready_queue;
int num_p, num_c;

void catcher(int signum)
{
    if (signum == SIGUSR1)
    {
        sigset_t myset;
        sigemptyset(&myset);
        printf("Suspending thread\n");
        sigsuspend(&myset);
    }
    else
    {
        printf("Waking thread\n");
        return;
    }
}

void* producer(void *param)
{
    signal(SIGUSR1, catcher);
    signal(SIGUSR2, catcher);
    
    int i = 0;
    while(i < 1000)
        if (pos < M-1)
        {
            pos++;
            BUFFER[pos] = rand() % 1000;
            i+=1;
        }

    vector<pthread_t>::iterator it;
    for (it = ready_queue.begin(); it != ready_queue.end(); it++, i++)
    {
        if (ready_queue[i] == pthread_self())
        {
            ready_queue.erase(ready_queue.begin() + i);
            break;
        }
    }
}

void* consumer(void *param)
{
    signal(SIGUSR1, catcher);
    signal(SIGUSR2, catcher);
    
    while(1)
    {
        if (pos > 0)
            pos--;
    }

    vector<pthread_t>::iterator it;
    int i = 0;
    for (it = ready_queue.begin(); it != ready_queue.end(); it++, i++)
    {
        if (ready_queue[i] == pthread_self())
        {
            ready_queue.erase(ready_queue.begin() + i);
            break;
        }
    }
    pthread_exit(0);
}

void* schedule(void* param)
{
    int i;
    for (i = 0; i < num_p; i++)
        pthread_kill(P_threads[i], SIGUSR1);

    for (i = 0; i < num_c; i++)
        pthread_kill(C_threads[i], SIGUSR1);

    int ready_queue[N];

    for (i = 0; i < num_p; i++)
        ready_queue.push_back(P_threads[i]);

    for (i = 0; i < num_c; i++)
        ready_queue.push_back(C_threads[i]);

    while(1)
    {
        if (!ready_queue.size())
            break;
        pthread_t top = ready_queue[0];
        pthread_kill(top, SIGUSR2);
        ready_queue.erase(ready_queue.begin());
        ready_queue.push_back(top);
        sleep(QUANTUM);
        pthread_kill(top, SIGUSR1);
    }
    pthread_exit(0);

}

int main()
{
    srand(time(0));
    
    num_p = rand() % N;
    num_c = N - num_p;
    
    pthread_t scheduler;

    for(int i = 0; i < num_p; i++)
    {
        pthread_attr_t attr_P;
        pthread_attr_init(&attr_P);
        pthread_create(&P_threads[i],&attr_P,producer,NULL); 
    }
    for(int i = 0; i < num_c ; i++)
    {

        pthread_attr_t attr_C;
        pthread_attr_init(&attr_C);
        pthread_create(&C_threads[i],&attr_C,consumer,NULL);          
    }
    pthread_create(&scheduler, NULL, schedule, NULL);

    for(int i =0; i<num_p; i++)
    {
        pthread_join(P_threads[i],NULL);
    }
    for(int i =0; i<num_c; i++)
    {
        pthread_join(C_threads[i],NULL);
    }

}
