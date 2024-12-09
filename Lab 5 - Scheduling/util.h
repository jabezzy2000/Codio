#ifndef UTIL_H
#define UTIL_H

#include "process.h"

/**
 * Utility function file
 */
void findWaitingTime(ProcessType plist[], int n);
void findTurnAroundTime(ProcessType plist[], int n);

ProcessType *parse_file(FILE *, int *);

#endif				// UTIL_H