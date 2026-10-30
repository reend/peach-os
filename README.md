# PeachOS

PeachOS is a 32-bit x86 hobby kernel. It boots from a FAT16 disk image, loads an ELF program into its own address space, and runs it in ring 3. A program talks to the kernel through `int 0x80`. A hardware timer switches between tasks.

The first program is `shell`. It reads a line, loads the named ELF from the disk (for example `blank.elf`), and the kernel starts that process. When a program returns from `main`, it exits and the scheduler runs the next task.

## Build and run

The kernel and user programs are built with an `i686-elf` cross compiler. `./build.sh` puts `$HOME/opt/cross/bin` on `PATH` and runs `make all`.

`make all` builds the boot sector and the kernel, links `shell` and `blank`, and writes `bin/os.bin`. The image is the boot sector, the kernel, and 16 MiB of zeros formatted as FAT16. `hello.txt`, `blank.elf`, and `shell.elf` are copied into that filesystem through a temporary mount at `/mnt/d`.

```sh
./build.sh
make run
```

`make run` starts QEMU with `bin/os.bin` as the hard disk. GDB symbols for the kernel live in `build/kernelfull.o`, which is linked at address 0. The binary that actually runs is `bin/kernel.bin`, placed at 1 MiB.

## Boot

```mermaid
flowchart LR
    BIOS[BIOS] --> Boot["boot.asm at 0x7C00"]
    Boot --> PM[Protected mode]
    PM --> Kernel["kernel.bin at 0x100000"]
    Kernel --> Shell["shell.elf"]
    Shell --> Program["program ELF"]
```

1. The BIOS loads the first sector to `0x7C00`. That sector is a FAT16 boot sector.
2. `src/boot/boot.asm` enables the A20 line, loads the kernel from disk to `0x100000`, and jumps to it in 32-bit protected mode. Interrupts are off.
3. `src/kernel.asm` sets the kernel segments and the boot stack at `0x200000`, remaps the PIC so hardware IRQ0 arrives as interrupt `0x20`, and calls `kernel_main`.
4. `kernel_main` loads the GDT, creates the kernel heap, mounts FAT16, installs the IDT, loads a TSS, turns on paging, and loads `0:/shell.elf`.
5. `task_return` builds an `iretd` frame and enters the shell at its ELF entry point, in ring 3.

## Memory map

| Address | What |
| --- | --- |
| `0x7C00` | Boot sector |
| `0x7E00` | Kernel heap block table |
| `0xB8000` | VGA text buffer |
| `0x100000` | Kernel |
| `0x200000` | Boot stack |
| `0x3FB000`–`0x3FF000` | User stack, 16 KiB, grows down |
| `0x400000` | User program |
| `0x600000` | Kernel stack used when a program enters the kernel |
| `0x01000000` | Kernel heap, 100 MiB |

The kernel linker script (`src/linker.ld`) sets `. = 1M` and emits a raw binary. User programs use `programs/*/linker.ld`: ELF32, entry `_start`, load address `0x400000`, each section aligned to a page.

## Layout

| Path | Role |
| --- | --- |
| `src/boot` | Real-mode bootloader |
| `src/gdt`, `src/task/tss.asm` | Segments and the one kernel TSS |
| `src/idt` | Interrupt table, syscall entry, timer |
| `src/memory` | Heap and paging |
| `src/disk`, `src/fs` | ATA disk, path parser, FAT16 |
| `src/loader/formats` | ELF loader |
| `src/task` | Processes and tasks |
| `src/isr80h` | `int 0x80` commands |
| `src/keyboard` | PS/2 keyboard |
| `programs/stdlib` | C runtime and syscall wrappers shared by every program |
| `programs/shell` | The program the kernel starts |
| `programs/blank` | A small program the shell can launch |

## Privilege

The GDT has a null segment, kernel code (`0x08`), kernel data (`0x10`), user code (`0x1B`), user data (`0x23`), and a TSS (`0x28`). Kernel code runs in ring 0. Programs run in ring 3.

There is one TSS. Only `esp0` and `ss0` are filled: stack `0x600000`, kernel data segment. `ltr` tells the CPU which GDT entry that TSS is. On a ring change the CPU switches to that stack by itself and pushes the return frame (`eip`, `cs`, `eflags`, and the user `esp`/`ss`). The program's own stack stays at `0x3FF000`.

`task_return` pushes the same kind of frame by hand, reloads the general-purpose registers from the task, and executes `iretd`. That is how a task starts. Coming back from a normal syscall is the `iretd` at the end of `isr80h_wrapper`.

## Paging

Paging is identity-mapped when it is turned on. The kernel was already running at physical addresses, so each virtual page in the kernel directory points at the same physical page. Kernel pointers stay valid.

Each task gets its own page directory from `paging_new_4gb`. Those pages are present and reachable from ring 3, and they are not writable. The loader then maps the pieces the program may use:

- ELF `PT_LOAD` segments at the virtual addresses stored in the file, with write permission only when the segment has `PF_W`.
- The 16 KiB stack at `0x3FB000`.
- Later `malloc` blocks, one mapping per allocation.

The same virtual addresses are used for every program. They do not collide because each task has its own directory. `task_switch` loads that directory. `kernel_page` loads the kernel directory again before the kernel touches its own pointers.

A pointer passed in from a program is a virtual address in that task's directory. The kernel translates it with `task_virtual_address_to_physical` and then uses the result directly, which works because the kernel directory is an identity map.

CPU exceptions `0`–`31` terminate the current process. Interrupt `0x20` is the timer and calls `task_next`.

## Processes and tasks

A process is the program: the loaded file, the stack, the allocations, the keyboard buffer, and `argc`/`argv`. At most 12 processes exist, in a fixed array. A task is the thing the CPU runs: registers plus a page directory. Each process has one task. Tasks form a list, and `task_next` advances to the next one.

Loading a file (`process_load`) does the following:

1. Take a free process slot.
2. Read the file. If it is a valid ELF executable, keep the ELF image and remember its entry point. Otherwise treat the bytes as a raw binary that starts at `0x400000`.
3. Allocate a 16 KiB stack.
4. Create a task whose `eip` is the ELF entry, `esp` is `0x3FF000`, and `cs`/`ss` are the user selectors.
5. Map the program and the stack into that task's page directory.

`process_load_switch` also marks the new process as the current one. The caller still has to `task_switch` and `task_return` to actually run it. The shell syscall does both. The timer path uses `task_next`, which saves nothing by itself: the interrupt stub already stored the interrupted registers in the task before the handler ran.

## System calls

A user program raises `int 0x80`. The IDT entry for `0x80` is `isr80h_wrapper`. The command number is in `eax`. Arguments are on the user stack; the kernel reads them through the saved user `esp`, not by popping its own stack. The return value goes back in `eax`.

| `eax` | Command | What it does |
| --- | --- | --- |
| 0 | sum | Adds the two integers on the user stack |
| 1 | print | Copies a user string and writes it to the VGA terminal |
| 2 | getkey | Pops one byte from the process keyboard buffer |
| 3 | putchar | Prints one character |
| 4 | malloc | Allocates memory and maps it into the process |
| 5 | free | Unmaps and frees a process allocation |
| 6 | load and start | Loads `0:/<name>` and `iretd`s into it |
| 7 | system | Same as 6, and copies the argument list into the new process |
| 8 | get arguments | Writes the process `argc` and `argv` into a user struct |
| 9 | exit | Terminates the current process and switches to the next task |

## User space

`programs/stdlib` is linked into every program. `start.asm` is the ELF entry: it calls `c_start`, and `c_start` calls `main(argc, argv)`. Those arguments come from syscall 8, which reads what the kernel stored when the process was created. After `main` returns, `start.asm` calls `peachos_exit`.

`peachos.asm` is the other side of the syscall table: `print` puts `1` in `eax`, `getkey` puts `2`, and so on. `peachos_parse_command` splits a command line on spaces into a linked list of `command_argument`. `peachos_system_run` parses a line and invokes syscall 7.

`shell` prints a prompt, reads a line with the keyboard, and passes that line to `peachos_system_run`. `blank` prints its arguments in a loop so a launched program is visible.

## Disk and keyboard

The disk driver talks to the primary ATA controller. `fs/file.c` opens paths of the form `0:/name`. The `0:` is the drive number. FAT16 directory entries are 8.3 names, compared without case sensitivity.

The keyboard driver is a PS/2 scan-code set 1 decoder. Bytes are queued in a ring buffer on the current process. A backspace drops the last queued byte. `getkey` dequeues one byte for the running task.
