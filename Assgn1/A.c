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
int sort(int* arr_here1, int* arr_here2, int m, int n)
{

    int i=0,j=m,k=0;
    
    while(i<m&&j<m+n)
    {
        if(arr_here1[i]<arr_here1[j])
        {
            arr_here2[k++] = arr_here1[i++];
        }
        else
        {
            arr_here2[k++] = arr_here1[j++];
        }
    }
    while(i<m)
    {
        arr_here2[k++] = arr_here1[i++];
    }
    while(j<m+n)
    {
        arr_here2[k++] = arr_here1[j++];
    }
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

  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd_a_d[2];  // Used to store two ends of first pipe 
    int fd_b_d[2];  // Used to store two ends of first pipe 
    int fd_c_e[2];  // Used to store two ends of first pipe 
    int fd_d_e[2];  // Used to store two ends of first pipe 
    int i;

    pid_t p; 
  
    if (pipe(fd_a_d)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    }
    if (pipe(fd_b_d)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    }
    if (pipe(fd_c_e)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    }
    if (pipe(fd_d_e)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
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
            int arr[50];

            for (i = 0; i < 50; i++)
                arr[i] = rand();

            quickSort(arr, 0, 49);

            printf("I am A\n");
      
            close(fd_a_d[0]);  // Close reading end of first pipe 
      
            // Write input string and close writing end of first 
            // pipe. 
            write(fd_a_d[1], arr, sizeof(int) * 50); 
            close(fd_a_d[1]); 

            exit(0);
        
        }

        else
        {
            printf("I am B.\n");
            int arr2[50];
            srand(2);
            for (i = 0; i < 50; i++)
                arr2[i] = rand();

            quickSort(arr2, 0, 49);

            // for (i = 0; i < 50; i++)
            //     printf("Org: %d\n", arr2[i]);

            close(fd_b_d[0]);  // Close reading end of first pipe 
        
            // Write input string and close writing end of first 
            // pipe. 
            write(fd_b_d[1], arr2, sizeof(int) * 50);
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
            
            int arr_here1[100];
            close(fd_a_d[1]);  // Close writing end of first pipe 
      
            // Read a string using first pipe 
            read(fd_a_d[0], arr_here1, sizeof(int) * 50);
      
            close(fd_a_d[0]);

            close(fd_b_d[1]);  // Close writing end of first pipe 
            wait(NULL);
            // Read a string using first pipe 
            read(fd_b_d[0], arr_here1+50, sizeof(int) * 50);
      
            close(fd_b_d[0]); 
      
            // for (i = 0; i < 50; i++)
            //     printf("First: %d\n", arr_here1[i]);


            // for (i = 0; i < 50; i++)
            //     printf("Second: %d\n", arr_here1[50+i]);
            int arr_here2[100];
            sort(arr_here1, arr_here2, 50, 50);

            close(fd_d_e[0]);  // Close reading end of first pipe 
            
            // Write input string and close writing end of first 
            // pipe. 
            write(fd_d_e[1], arr_here2, sizeof(int) * 100); 
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
                int arr[50];
                srand(3);
                for (i = 0; i < 50; i++)
                    arr[i] = rand();

                quickSort(arr, 0, 49);

                // for (i = 0; i < 50; i++)
                //     printf("Org: %d\n", arr2[i]);

                close(fd_c_e[0]);  // Close reading end of first pipe 
                
                // Write input string and close writing end of first 
                // pipe. 
                write(fd_c_e[1], arr, sizeof(int) * 50);
                close(fd_c_e[1]);

                exit(0);
            }
            else
            {
                printf("I am E\n");
                int arr_here[150];

                close(fd_d_e[1]);  // Close writing end of first pipe 
                // Read a string using first pipe 
                read(fd_d_e[0], arr_here, sizeof(int) * 100);
                
                close(fd_d_e[0]);

                close(fd_c_e[1]);  // Close writing end of first pipe 
                wait(NULL);
                // Read a string using first pipe 
                read(fd_c_e[0], arr_here+100, sizeof(int) * 50);
                
                close(fd_c_e[0]); 

                int arr_here3[150];
                sort(arr_here, arr_here3, 100, 50);

                for (i = 0; i < 150; i++)
                    printf("Final: %d\n", arr_here3[i]);

                exit(0);
                

            }
        }


  
    }  
} 