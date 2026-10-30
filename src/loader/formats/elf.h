#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>

#define PF_X 0x01 // executable segment
#define PF_W 0x02 // writable segment
#define PF_R 0x04 // readable segment

#define PT_NULL 0    // unused program header
#define PT_LOAD 1    // load this segment into memory
#define PT_DYNAMIC 2 // dynamic linking information
#define PT_INTERP 3  // path to the interpreter
#define PT_NOTE 4    // auxiliary information
#define PT_SHLIB 5   // reserved
#define PT_PHDR 6    // the program header table itself

#define SHT_NULL 0     // unused section
#define SHT_PROGBITS 1 // code or initialized data
#define SHT_SYMTAB 2   // symbol table
#define SHT_STRTAB 3   // string table
#define SHT_RELA 4     // relocations with an addend
#define SHT_HASH 5     // symbol hash table
#define SHT_DYNAMIC 6  // dynamic linking information
#define SHT_NOTE 7     // auxiliary information
#define SHT_NOBITS 8   // occupies memory but not the file, like .bss
#define SHT_REL 9      // relocations without an addend
#define SHT_SHLIB 10   // reserved
#define SHT_DYNSYM 11  // dynamic symbol table
#define SHT_LOPROC 12  // first processor-specific section type
#define SHT_HIPROC 13  // last processor-specific section type
#define SHT_LOUSER 14  // first application-specific section type
#define SHT_HIUSER 15  // last application-specific section type

#define ET_NONE 0 // no file type
#define ET_REL 1  // relocatable object file
#define ET_EXEC 2 // executable file
#define ET_DYN 3  // shared object
#define ET_CORE 4 // core dump

#define EI_NIDENT 16 // size of e_ident, in bytes
#define EI_CLASS 4   // index of the class byte inside e_ident
#define EI_DATA 5    // index of the endianness byte inside e_ident

#define ELFCLASSNONE 0 // invalid class
#define ELFCLASS32 1   // 32-bit objects
#define ELFCLASS64 2   // 64-bit objects

#define ELFDATANONE 0 // invalid data encoding
#define ELFDATA2LSB 1 // little-endian
#define ELFDATA2MSB 2 // big-endian

#define SHN_UNDEF 0 // undefined section index

typedef uint16_t elf32_half;
typedef uint32_t elf32_word;
typedef int32_t elf32_sword;
typedef uint32_t elf32_addr;
typedef int32_t elf32_off;

// One loadable segment, an entry of the program header table.
struct elf32_phdr
{
    elf32_word p_type;   // PT_LOAD, PT_DYNAMIC, ...
    elf32_off p_offset;  // where this segment starts in the file
    elf32_addr p_vaddr;  // virtual address to map it at
    elf32_addr p_paddr;  // physical address, usually unused
    elf32_word p_filesz; // bytes to read from the file
    elf32_word p_memsz;  // bytes to reserve in memory
    elf32_word p_flags;  // PF_R, PF_W, PF_X
    elf32_word p_align;  // required alignment
} __attribute__((packed));

// One section, an entry of the section header table.
struct elf32_shdr
{
    elf32_word sh_name;      // offset of the name in the section string table
    elf32_word sh_type;      // SHT_PROGBITS, SHT_SYMTAB, ...
    elf32_word sh_flags;     // writable, allocatable, executable
    elf32_addr sh_addr;      // virtual address when the section is loaded
    elf32_off sh_offset;     // where this section starts in the file
    elf32_word sh_size;      // size in bytes
    elf32_word sh_link;      // index of an associated section
    elf32_word sh_info;      // extra info, meaning depends on sh_type
    elf32_word sh_addralign; // required alignment
    elf32_word sh_entsize;   // size of one table entry, or 0
} __attribute__((packed));

// File header at the start of an ELF file.
struct elf_header
{
    unsigned char e_ident[EI_NIDENT]; // magic, class, endianness
    elf32_half e_type;                // ET_EXEC, ET_REL, ET_DYN, ...
    elf32_half e_machine;             // target instruction set
    elf32_word e_version;             // ELF version, 1 means current
    elf32_addr e_entry;               // virtual address of the entry point
    elf32_off e_phoff;                // file offset of the program header table
    elf32_off e_shoff;                // file offset of the section header table
    elf32_word e_flags;               // architecture-specific flags
    elf32_half e_ehsize;              // size of this header
    elf32_half e_phentsize;           // size of one program header
    elf32_half e_phnum;               // number of program headers
    elf32_half e_shentsize;           // size of one section header
    elf32_half e_shnum;               // number of section headers
    elf32_half e_shstrndx; // section index of the section name string table
} __attribute__((packed));

// One dynamic-linking entry.
struct elf32_dyn
{
    elf32_sword d_tag; // which kind of entry this is
    union
    {
        elf32_word d_val; // integer value
        elf32_addr d_ptr; // address value
    } d_un;

} __attribute__((packed));

// One symbol.
struct elf32_sym
{
    elf32_word st_name;     // offset of the name in the string table
    elf32_addr st_value;    // address or offset of the symbol
    elf32_word st_size;     // size of the object, in bytes
    unsigned char st_info;  // binding and symbol type
    unsigned char st_other; // visibility, usually zero
    elf32_half st_shndx;    // section the symbol belongs to
} __attribute__((packed));

void *elf_get_entry_ptr(struct elf_header *elf_header);
uint32_t elf_get_entry(struct elf_header *elf_header);

#endif
