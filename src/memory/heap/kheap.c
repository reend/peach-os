#include "kheap.h"
#include "heap.h"
#include "../../config.h"
#include "../../kernel.h"
#include "../memory.h"

struct heap_table kernel_heap_table;
struct heap kernel_heap;

void kheap_init()
{
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)(HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE;

    void* end = (void*)(HEAP_ADDRESS + HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)(HEAP_ADDRESS), end, &kernel_heap_table);

    if (res < 0)
    {
        print("Failed to create heap\n");
    }
}

void* kmalloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(size_t size)
{
    void* ptr = kmalloc(size);

    if (!ptr)
    {
        return 0;
    }

    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void *ptr)
{
    heap_free(&kernel_heap, ptr);
}