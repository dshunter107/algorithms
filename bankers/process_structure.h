#include "process_structure.c"

#define PROCESSES 10

#ifndef PROCESS_STRUCTURE_H
#define PROCESS_STRUCTURE_H

extern struct process max[PROCESSES];
extern struct process queue[PROCESSES];
extern struct process next;
#endif
