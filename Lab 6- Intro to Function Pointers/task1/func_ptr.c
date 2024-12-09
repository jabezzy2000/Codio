#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include "process.h"
#include "util.h"

#define DEBUG 0 // Change this to 1 to enable verbose output

/**
 * Signature for a function pointer that can compare.
 * You need to cast the input into its actual 
 * type and then compare them according to your
 * custom logic.
 */
typedef int (*Comparer)(const void *a, const void *b);

/**
 * Compares two processes.
 * Sorting logic:
 * - First by priority (ascending).
 * - Then by arrival time (ascending) in case of ties.
 */
int my_comparer(const void *this, const void *that)
{
    // Cast the pointers to `Process` type
    Process *p1 = (Process *)this;
    Process *p2 = (Process *)that;

    // Compare based on priority
    if (p1->priority != p2->priority) {
        return p1->priority - p2->priority;
    }

    // If priorities are the same, compare based on arrival time
    return p1->arrival_time - p2->arrival_time;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./func-ptr <input-file-path>\n");
        fflush(stdout);
        return 1;
    }

    /*******************/
    /* Parse the input */
    /*******************/
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        return 1;
    }

    Process *processes = parse_file(input_file);

    /*******************/
    /* Sort the input  */
    /*******************/
    Comparer process_comparer = &my_comparer;

#if DEBUG
    for (int i = 0; i < P_SIZE; i++) {
        printf("%d (%d, %d) ",
               processes[i].pid,
               processes[i].priority, processes[i].arrival_time);
    }
    printf("\n");
#endif

    qsort(processes, P_SIZE, sizeof(Process), process_comparer);

    /**************************/
    /* Print the sorted data  */
    /**************************/
    for (int i = 0; i < P_SIZE; i++) {
        printf("%d (%d, %d)\n",
               processes[i].pid,
               processes[i].priority, processes[i].arrival_time);
    }
    fflush(stdout);
    fflush(stderr);

    /************/
    /* Clean up */
    /************/
    free(processes);
    fclose(input_file);
    return 0;
}
