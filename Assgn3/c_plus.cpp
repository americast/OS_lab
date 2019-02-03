#include <iostream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <queue>
#include <math.h>

#define E 2.731
#define MEAN 5

using namespace std;

int curr_time = 0;
typedef struct process
{
    int id,arr_time,CPU_burst;
}process;

bool operator<(const process& p1, const process& p2) 
{ 
    return p1.CPU_burst > p2.CPU_burst; 
}  

void genCPUBursts(vector<process> &P , int N)
{
    for(int i = 0; i<N; i++)
    {
        P[i].id = i+1;
        P[i].CPU_burst =(rand()%20)+1 ; 
    }
}

void genArrTime(vector<process> &P , int N)
{
    P[0].arr_time = 0;
    srand(time(0));
    for(int i = 1; i<N; i++)
    {

        // float R = (rand()%100000)/100000.0;
        // cout<<R<<"   :    ";
        // cout<<(-1.0 / MEAN ) * log(R) /log(E)<<" \n";

        P[i].arr_time = P[i-1].arr_time+(rand()%10)+1;
    }
}

float atn_fcfs(vector<process> P , int N)
{
    int i ,curr_time = 0, atn = 0;
    cout<<"\n\nFCFS start\n";
    for( i = 0; i< N; i++)
    {
        atn = atn + (curr_time - P[i].arr_time) + P[i].CPU_burst;
        if(i+1<N)
        {
            cout<< "Process Num : "<<P[i].id<<" start time : "<<curr_time<<"\n";
            curr_time = (P[i+1].arr_time>curr_time+P[i].CPU_burst)? P[i+1].arr_time : curr_time+P[i].CPU_burst ; 
        }
        else
        {
            cout<< "Process Num : "<<P[i].id<<" start time : "<<curr_time<<"\n";
            curr_time = curr_time+P[i].CPU_burst;
        }
    }
    cout<<"FCFS end\n";
    return((float)atn/N);
}

float atn_nsjf(vector<process> P ,  int N)
{
    int i, curr_time = 0, atn = 0;
    cout<<"\n\nnNSJF start\n";
    priority_queue <process> pq;
    for( i = 0; i< N; )
    {
        while(i<N && P[i].arr_time<=curr_time)
        {
            pq.push(P[i]);
            i++;
        }
        if(!pq.size())
        {
            curr_time = P[i].arr_time;
            pq.push(P[i++]);
        }
        process P = pq.top();
        pq.pop();
        cout<< "Process Num : "<<P.id<<" start time : "<<curr_time<<"\n";
        atn = curr_time - P.arr_time + P.CPU_burst;
        curr_time = curr_time + P.CPU_burst;
    }
    while(pq.size())
    {
        process P = pq.top();
        pq.pop();
        cout<< "Process Num : "<<P.id<<" start time : "<<curr_time<<"\n";
        atn = curr_time - P.arr_time + P.CPU_burst;
        curr_time = curr_time + P.CPU_burst;
    }

    cout<<"nNSJF end\n";
    return((float)atn/N);
}

float atn_sjf(vector<process> P ,  int N)
{
    int i = 0, curr_time = 0, atn = 0,count = 0;
    cout<<"\n\npNSJF start\n";
    priority_queue <process> pq;
    while(count<N)
    {
        while(i<N&&P[i].arr_time<=curr_time)
        {
            cout<<"pushing "<<P[i].id<<" in pq at arr time "<<P[i].arr_time<<" curr time "<<curr_time<<"\n";
            pq.push(P[i]);
            i++;
        }
        if(pq.size())
        {
            process F = pq.top();
            pq.pop();
            cout<<"doing process "<<F.id<<" at "<<curr_time<<"\n";
            if(F.CPU_burst < 2)
            {
                cout<<"done: "<<F.id<<"\n";
                count++;
                atn = curr_time + 1 - F.arr_time;
            }
            else
            {
                F.CPU_burst--;
                pq.push(F);
            }
        }
        curr_time++;
    }
    cout<<"\n\npSJF end\n";
    return((float)atn/N);
}

float atn_rr(vector<process> P ,  int N)
{
    int i = 0, curr_time = 0, atn = 0,count = 0;
    cout<<"\n\nRR start\n";
    vector<process> ready;
    int rem = 0;
    while(count<N)
    {
        while(i<N && P[i].arr_time<=curr_time)
        {
            cout<<"pushing "<<P[i].id<<" in pq at arr_time "<<P[i].arr_time<<"   curr_time"<<curr_time<<"\n";
            ready.push_back(P[i]);
            i++;
        }
        if(ready.size())
            rem = rem % ready.size();
        else
            rem = 0;
        if(ready.size())
        {           
            cout<<"doing process "<<ready[rem].id<<" at "<<curr_time<<"\n";
            if(ready[rem].CPU_burst <= 2)
            {
                cout<<"done: "<<ready[rem].id<<"\n";
                count++;
                atn = curr_time + ready[rem].CPU_burst - ready[rem].arr_time;
                curr_time += ready[rem].CPU_burst;
                ready.erase(ready.begin()+rem);
            }
            else
            {
                ready[rem].CPU_burst -= 2;
                curr_time += 2;
                rem ++;
            }
        }
        else if(i<N)
        {
            curr_time = P[i].arr_time;
            rem = 0;
        }
    }
    cout<<"\n\nRR end\n";
    return((float)atn/N);
}

bool compareRN(process i1, process i2) 
{ 
    float rn1 = (float)(curr_time-i1.arr_time+i1.CPU_burst)/i1.CPU_burst;
    float rn2 = (float)(curr_time-i2.arr_time+i2.CPU_burst)/i2.CPU_burst;
    return (rn1 > rn2); 
} 

float atn_hrn(vector<process> P ,  int N)
{
    int i = 0 , atn = 0, count = 0;
    curr_time  = 0;
    cout<<"\n\nHRN start\n";
    vector <process> pq;

    while(count<N)
    {
        while(i<N && P[i].arr_time<=curr_time)
        {
            pq.push_back(P[i]);
            i++;
        }
        
        if(i<N && !pq.size())
        {
            curr_time = P[i].arr_time;
            pq.push_back(P[i++]);
        }
        sort(pq.begin(),pq.end(),compareRN);
        for(int j =0; j<pq.size(); j++)
        {
            cout<<pq[j].id<<"\n";
        }
        process P = pq[0];
        pq.erase(pq.begin());
        count++;
        cout<< "Process Num : "<<P.id<<" start time : "<<curr_time<<"\n";
        atn = curr_time - P.arr_time + P.CPU_burst;
        curr_time = curr_time + P.CPU_burst;
    }
    while(pq.size())
    {
        process P = pq[0];
        pq.erase(pq.begin());
        cout<< "Process Num : "<<P.id<<" start time : "<<curr_time<<"\n";
        atn = curr_time - P.arr_time + P.CPU_burst;
        curr_time = curr_time + P.CPU_burst;
    }

    cout<<"HRN end\n";
    return((float)atn/N);
    // int i = 0, atn = 0,count = 0;
    // curr_time = 0;
    // cout<<"\n\nhrn start\n";
    // vector <process> pq;
    // while(count<N)
    // {
    //     while(i<N&&P[i].arr_time<=curr_time)
    //     {
    //         cout<<"pushing "<<P[i].id<<" in pq at arr time "<<P[i].arr_time<<" curr time "<<curr_time<<"\n";
    //         pq.push_back(P[i]);
    //         i++;
    //     }
    //     sort(pq.begin(), pq.end(), compareRN);
    //     if(pq.size())
    //     {
    //         // process F = ;
    //         // pq.pop_front();
    //         cout<<"doing process "<<pq[0].id<<" at "<<curr_time<<"and hrn "<<(float)(curr_time-pq[0].arr_time+pq[0].CPU_burst)/pq[0].CPU_burst<<"\n";
    //         if(pq[0].CPU_burst < 2)
    //         {
    //             cout<<"done: "<<pq[0].id<<"\n";
    //             count++;
    //             atn = curr_time + 1 - pq[0].arr_time;
    //             pq.erase(pq.begin()+0);
    //         }
    //         else
    //         {
    //             pq[0].CPU_burst--;
    //             // pq.push_back(F);
    //         }
    //     }
    //     curr_time++;
    // }
    // cout<<"\n\nhrn end\n";
    // return((float)atn/N);
}

int main() 
{
    while(1)
    {
        srand(time(0));

        cout<<"\nEnter the numer of random numbers to be generated:\n";
        cout<<"Enter -1 to quit \n";
        int i, N;
        cin>>N;
        vector<process> P(N);
        if(N==-1)
        {
            cout<<"Exiting program\n";
            exit(0);
        }
        if(N<1)
        {
            cout<<"Wrong input!!!\n";
            continue;
        }

        genCPUBursts(P,N);
        genArrTime(P,N);
        FILE *table = fopen("table.txt", "w");
        if(table)
        {
            fprintf( table , "\tProcess Number\tCPU Burst\tInter Arrival Time\t\n" );
            for(i = 0; i<N; i++)
            {
                fprintf( table , "\t%d\t\t%d\t\t%d\t\n" , P[i].id , P[i].CPU_burst , P[i].arr_time );
            }
            fclose(table);
        }
        else
        {
            perror("Error in opening output file : \n");
        }

        cout<<"Non-preemptive First Come First Serve (FCFS)     : "<<atn_fcfs(P , N)<<"\n";
        cout<<"Non-preemptive Shortest Job First                : "<<atn_nsjf(P , N)<<"\n";
        cout<<"Pre-emptive Shortest Job First                   : "<<atn_sjf(P , N)<<"\n";
        cout<<"Round Robin with time quantum δ = 2 time units   : "<<atn_rr(P , N)<<"\n";
        cout<<"Highest response-ratio next (HRN)                : "<<atn_hrn(P , N)<<"\n";

        // vector<process> v(5);

        // v[0].id = 1;
        // v[0].arr_time = 0;
        // v[0].CPU_burst = 3;

        // v[1].id = 2;
        // v[1].arr_time = 2;
        // v[1].CPU_burst = 6;

        // v[2].id = 3;
        // v[2].arr_time = 4;
        // v[2].CPU_burst = 4;

        // v[3].id = 4;
        // v[3].arr_time = 6;
        // v[3].CPU_burst = 5;

        // v[4].id = 5;
        // v[4].arr_time = 8;
        // v[4].CPU_burst = 2;

        // cout<<"Non-preemptive First Come First Serve (FCFS)     : "<<atn_fcfs(v , v.size())<<"\n";
        // // cout<<"Non-preemptive Shortest Job First                : "<<atn_nsjf(v , v.size())<<"\n";
        // // cout<<"Pre-emptive Shortest Job First                   : "<<atn_sjf(v , v.size())<<"\n";
        // // cout<<"Round Robin with time quantum δ = 2 time units   : "<<atn_rr(v , v.size())<<"\n";
        // cout<<"Highest response-ratio next (HRN)                : "<<atn_hrn(v , v.size())<<"\n";



    }
}
