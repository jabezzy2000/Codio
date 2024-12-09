// C program for implementation of Simulation 
#include<stdio.h> 
#include<limits.h>
#include<stdlib.h>
#include "process.h"
#include "util.h"

// Function to find the waiting time for all processes (Round Robin)
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    int remaining_bt[n], t = 0;

    for (int i = 0; i < n; i++) {
        remaining_bt[i] = plist[i].bt;
    }

    int done;
    do {
        done = 1;
        for (int i = 0; i < n; i++) {
            if (remaining_bt[i] > 0) {
                done = 0; // Process still pending

                if (remaining_bt[i] > quantum) {
                    t += quantum;
                    remaining_bt[i] -= quantum;
                } else {
                    t += remaining_bt[i];
                    plist[i].wt = t - plist[i].bt;
                    remaining_bt[i] = 0;
                }
            }
        }
    } while (!done);
} 

// Function to find the waiting time for all processes (Shortest Job First)
void findWaitingTimeSJF(ProcessType plist[], int n)
{ 
    int completed = 0, current_time = 0, min_bt_index;
    int remaining_bt[n];
    int is_completed[n];

    for (int i = 0; i < n; i++) {
        remaining_bt[i] = plist[i].bt;
        is_completed[i] = 0;
    }

    while (completed != n) {
        min_bt_index = -1;

        // Find process with minimum remaining burst time
        for (int i = 0; i < n; i++) {
            if (!is_completed[i] && (min_bt_index == -1 || remaining_bt[i] < remaining_bt[min_bt_index])) {
                min_bt_index = i;
            }
        }

        // Process the job
        current_time += remaining_bt[min_bt_index];
        plist[min_bt_index].wt = current_time - plist[min_bt_index].bt - plist[min_bt_index].art;
        plist[min_bt_index].tat = plist[min_bt_index].wt + plist[min_bt_index].bt;
        remaining_bt[min_bt_index] = 0;
        is_completed[min_bt_index] = 1;
        completed++;
    }
} 

// Function to sort the Process by priority
int my_comparer(const void *this, const void *that)
{ 
    ProcessType *p1 = (ProcessType *)this;
    ProcessType *p2 = (ProcessType *)that;
    return p1->pri - p2->pri; // Lower priority value gets higher priority
}

// Function to calculate average time (First Come First Serve)
void findavgTimeFCFS(ProcessType plist[], int n) 
{ 
    // Function to find waiting time of all processes 
    findWaitingTime(plist, n); 
  
    // Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    // Display processes along with all details 
    printf("\n*********\nFCFS\n");
}

// Function to calculate average time (Shortest Job First)
void findavgTimeSJF(ProcessType plist[], int n) 
{ 
    // Function to find waiting time of all processes 
    findWaitingTimeSJF(plist, n); 
  
    // Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    // Display processes along with all details 
    printf("\n*********\nSJF\n");
}

// Function to calculate average time (Round Robin)
void findavgTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    // Function to find waiting time of all processes 
    findWaitingTimeRR(plist, n, quantum); 
  
    // Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    // Display processes along with all details 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

// Function to calculate average time (Priority)
void findavgTimePriority(ProcessType plist[], int n) 
{ 
    // Sort the processes by priority
    qsort(plist, n, sizeof(ProcessType), my_comparer);

    // Apply FCFS logic after sorting
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n);

    // Display processes along with all details 
    printf("\n*********\nPriority\n");
}

// Function to calculate and print metrics
void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
  
    // Calculate total waiting time and total turn around time 
    for (int i = 0; i < n; i++) 
    { 
        total_wt += plist[i].wt; 
        total_tat += plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

// Initialize processes from input file
ProcessType *initProc(char *filename, int *n) 
{
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}
  
// Driver code 
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;

    ProcessType *proc_list;
  
    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        fflush(stdout);
        return 1;
    }
    
    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);

    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n);
    printMetrics(proc_list, n);

    // Priority
    n = 0; 
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n);
    printMetrics(proc_list, n);

    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum);
    printMetrics(proc_list, n);
    
    return 0; 
} 