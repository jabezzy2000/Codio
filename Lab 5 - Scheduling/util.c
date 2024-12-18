#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

#include "util.h"
#include "process.h"

/**
 * Returns an array of process that are parsed from
 * the input file descriptor passed as argument
 * CAUTION: You need to free up the space that is allocated
 * by this function
 */
ProcessType *parse_file(FILE * f, int *P_SIZE)
{
	int i = 0;

  ProcessType *pptr = (ProcessType *) malloc(sizeof(ProcessType));
  
  // count the number of processes
  while (!feof(f)) {
		fscanf(f, "%d %d %d %d %d %d\n", &(pptr->pid), &(pptr->bt), &(pptr->art), &(pptr->wt), &(pptr->tat), &(pptr->pri));
    *P_SIZE += 1;
	}

  free(pptr);
  fseek(f, 0, SEEK_SET);  // reset file pointer to beginning of fils
  
	// read all the data
	pptr = (ProcessType *) calloc(*P_SIZE, sizeof(ProcessType));
	while (!feof(f)) {
		fscanf(f, "%d %d %d %d %d %d\n", &(pptr[i].pid), &(pptr[i].bt), &(pptr[i].art), &(pptr[i].wt), &(pptr[i].tat), &(pptr[i].pri));
		i++;
	}

	return pptr;
}

void findWaitingTime(ProcessType plist[], int n) {
    plist[0].wt = 0;  // The first process has no waiting time

    // Calculate waiting time for each subsequent process
    for (int i = 1; i < n; i++) {
        plist[i].wt = plist[i - 1].wt + plist[i - 1].bt;
    }
}

#include "util.h"

// Function to calculate turn around time for all processes
void findTurnAroundTime(ProcessType plist[], int n) {
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].wt + plist[i].bt;
    }
}
