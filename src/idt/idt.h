#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_desc
{
    uint16_t offset_1; // offset bits 0-15
    uint16_t selector; // selector thats in out GDT
    uint8_t zero; // unused set to zero
    uint8_t type_attr; // desc type and attr
    uint16_t offset_2; // offset bits 16-31
} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // size of desc table - 1
    uint32_t base; // base addr of the start of interrupt desc table
} __attribute((packed));

void idt_init();
void enable_interrupts();
void disable_interrupts();

#endif