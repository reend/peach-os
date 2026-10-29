#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "../config.h"
#include "task.h"

#define PROCESS_FILE_TYPE_ELF 0
#define PROCESS_FILE_TYPE_BINARY 1 

typedef unsigned char PROCESS_FILE_TYPE;

struct process_allocation
{   
    void* ptr;
    size_t size;
};

// only 1 task per process
struct process
{
    uint16_t id;
    char filename[MAX_PATH];
    struct task* task;
    struct process_allocation allocations[MAX_PROGRAM_ALLOCATIONS]; // malloc allocations of a process
    PROCESS_FILE_TYPE filetype;
    union 
    {
        void* ptr; // physical ptr to process memory
        struct elf_file* elf_file;
    };
    void* stack; // physical ptr to stack memory
    uint32_t size; // size of the data pointer by ptr
    
    struct keyboard_buffer
    {
        char buffer[KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

int process_switch(struct process* process);
int process_load_switch(const char* filename, struct process** process);
int process_load(const char* filename, struct process** process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);
struct process* process_current();
struct process* process_get(int process_id);
void* process_malloc(struct process* process, size_t size);
void process_free(struct process* process, void* ptr);

#endif