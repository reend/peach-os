#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "../config.h"
#include "task.h"

// only 1 task per process
// no elf here
struct process
{
    uint16_t id;
    char filename[MAX_PATH];
    struct task* task;
    void* allocations[MAX_PROGRAM_ALLOCATIONS]; // malloc allocations of a process
    void* ptr; // physical ptr to process memory
    void* stack; // physical ptr to stack memory
    uint32_t size; // size of the data pointer by ptr
};

int process_load(const char* filename, struct process** process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);

#endif