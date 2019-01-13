#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

void swap(int* a, int* b) 
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 
  
/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) 
   to left of pivot and all greater elements to right 
   of pivot */
int partition (int arr[], int low, int high) 
{ 
    int pivot = arr[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
    int j;
    for (j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than or 
        // equal to pivot 
        if (arr[j] <= pivot) 
        { 
            i++;    // increment index of smaller element 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
} 


/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(int arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
} 

void sort(int* arr1, int* arr2, int m, int n)
{

    int i=0,j=m,k=0;
    
    while(i<m&&j<m+n)
    {
        if(arr1[i]<arr1[j])
        {
            arr2[k++] = arr1[i++];
        }
        else
        {
            arr2[k++] = arr1[j++];
        }
    }
    while(i<m)
    {
        arr2[k++] = arr1[i++];
    }
    while(j<m+n)
    {
        arr2[k++] = arr1[j++];
    }
}

  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd_a_d[2];  // Pipe from process a to d 
    int fd_b_d[2];  // Pipe from process b to d
    int fd_c_e[2];  // Pipe from process c to e
    int fd_d_e[2];  // Pipe from process d to e
    int i;

    pid_t p; 
  
    if (pipe(fd_a_d)==-1) 
    { 
        fprintf(stderr, "Pipe A to D Failed" ); 
        return 1; 
    }
    if (pipe(fd_b_d)==-1) 
    { 
        fprintf(stderr, "Pipe B to D Failed" ); 
        return 1; 
    }
    if (pipe(fd_c_e)==-1) 
    { 
        fprintf(stderr, "Pipe C to E Failed" ); 
        return 1; 
    }
    if (pipe(fd_d_e)==-1) 
    { 
        fprintf(stderr, "Pipe D to E Failed" ); 
        return 1; 
    }

    p = fork(); 
  
    if (p < 0)
    { 
        fprintf(stderr, "Fork Failed" ); 
        return 1; 
    } 
    // Parent process 
    else if (p > 0) 
    { 

        pid_t p2;
        p2 = fork(); 
        
        if (p2 < 0)
        { 
            fprintf(stderr, "Fork Failed" ); 
            return 1; 
        } 
        // Parent process 
        else if (p2 > 0) 
        { 
            printf("I am A\n");
            int arr_a[50];

            for (i = 0; i < 50; i++)
                arr_a[i] = rand();

            quickSort(arr_a, 0, 49);

            printf("Elements in A are\n");
            for(i = 0; i < 50; i++)
                printf("A%d: %d\n", i+1, arr_a[i]);
      
            close(fd_a_d[0]);  // Close reading end of first pipe 
      
            // Write input string and close writing end of first 
            // pipe. 
            write(fd_a_d[1], arr_a, sizeof(int) * 50); 
            close(fd_a_d[1]); 

            exit(0);
        
        }

        else
        {
            printf("I am B.\n");

            int arr_b[50];
            srand(2);
            for (i = 0; i < 50; i++)
                arr_b[i] = rand();

            quickSort(arr_b, 0, 49);

            printf("Elements in B are\n");
            for(i = 0; i < 50; i++)
                printf("B%d: %d\n", i+1, arr_b[i]);

            close(fd_b_d[0]);  // Close reading end of first pipe 
        
            // Write input string and close writing end of first 
            // pipe. 
            write(fd_b_d[1], arr_b, sizeof(int) * 50);
            close(fd_b_d[1]);

            exit(0);

        }
  
    } 
    // child process 
    else
    { 
        pid_t p3;
        p3 = fork();

        if (p3 < 0)
            printf("Fork failed.\n");
        else if (p3 > 0)  // Parent
        {

            printf("I am D.\n");
            
            int arr_ab[100];
            close(fd_a_d[1]);  // Close writing end of first pipe 
      
            // Read a string using first pipe 
            printf("D reading from A\n");
            read(fd_a_d[0], arr_ab, sizeof(int) * 50);
      
            close(fd_a_d[0]);

            close(fd_b_d[1]);  // Close writing end of first pipe 
            wait(NULL);

            // Read a string using first pipe 
            printf("D reading from B\n");
            read(fd_b_d[0], arr_ab+50, sizeof(int) * 50);
      
            close(fd_b_d[0]); 
      
            int arr_d[100];
            sort(arr_ab, arr_d, 50, 50);

            close(fd_d_e[0]);  // Close reading end of first pipe 
            
            // Write input string and close writing end of first 
            // pipe. 
            printf("Elements in D:\n");
            for(int i =0; i<100;i++)
                printf("D%d: %d\n", i+1 ,arr_d[i]);
            write(fd_d_e[1], arr_d, sizeof(int) * 100); 
            close(fd_d_e[1]); 

            exit(0); 
            
        }
        else
        {
            pid_t p4;
            p4 = fork();

            if (p4 < 0)
                printf("Fork failed.\n");

            else if (p4 > 0)    // Parent
            {
                printf("I am C\n");

                int arr_c[50];
                srand(3);
                for (i = 0; i < 50; i++)
                    arr_c[i] = rand();

                quickSort(arr_c, 0, 49);

                printf("Elements in C:\n");
                for (i = 0; i < 50; i++)
                    printf("C%d: %d\n", i+1, arr_c[i]);

                close(fd_c_e[0]);  // Close reading end of first pipe 
                
                // Write input string and close writing end of first 
                // pipe. 
                write(fd_c_e[1], arr_c, sizeof(int) * 50);
                close(fd_c_e[1]);

                exit(0);
            }
            else
            {
                printf("I am E\n");
                int arr_cd[150];

                close(fd_c_e[1]);  // Close writing end of first pipe 
                
                // Read a string using first pipe 
                printf("E reading from C\n");
                read(fd_c_e[0], arr_cd+100, sizeof(int) * 50);
                
                close(fd_c_e[0]); 


                close(fd_d_e[1]);  // Close writing end of first pipe
                wait(NULL);

                // Read a string using first pipe 
                printf("E reading from D\n");
                read(fd_d_e[0], arr_cd, sizeof(int) * 100);
                
                close(fd_d_e[0]);


                int arr_e[150];
                sort(arr_cd, arr_e, 100, 50);

                printf("Elements in E\n");
                for (i = 0; i < 150; i++)
                    printf("E%d: %d\n", i+1, arr_e[i]);

                exit(0);
                

            }
        }


  
    }  
} 