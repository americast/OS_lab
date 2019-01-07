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

  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of first pipe 
    int i;

    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    }
    if (pipe(fd2)==-1) 
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
      
            close(fd1[0]);  // Close reading end of first pipe 
      
            // Write input string and close writing end of first 
            // pipe. 
            write(fd1[1], arr, sizeof(int) * 50); 
            close(fd1[1]); 
        
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

            close(fd2[0]);  // Close reading end of first pipe 
        
            // Write input string and close writing end of first 
            // pipe. 
            write(fd2[1], arr2, sizeof(int) * 50);
            close(fd2[1]);

        }
  
    } 
    // child process 
    else
    { 
        printf("I am D.\n");
        
        int arr_here1[50];
        close(fd1[1]);  // Close writing end of first pipe 
  
        // Read a string using first pipe 
        read(fd1[0], arr_here1, sizeof(int) * 50);
  
        close(fd1[0]);

        int arr_here2[50];
        close(fd2[1]);  // Close writing end of first pipe 
        wait(NULL);
        // Read a string using first pipe 
        read(fd2[0], arr_here2, sizeof(int) * 50);
  
        close(fd2[0]); 
  
        for (i = 0; i < 50; i++)
            printf("First: %d\n", arr_here1[i]);


        for (i = 0; i < 50; i++)
            printf("Second: %d\n", arr_here2[i]);

        exit(0); 


  
    }  
} 