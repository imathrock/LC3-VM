# LC3 Virtual Machine Project

I saw a linkedin post where some guy had made a simple esp32 emulator in C++ and I realized that I could make that myself and so i did. I decided C because why not, I found [Write your own VM](https://www.jmeiners.com/lc3-vm/) by Justin Meiners and Ryan Pendleton and decided to follow along with my own implementation, I did not copy paste obviously since for most of the time I was referring to [LC3 ISA description](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf) and our codes are quite different. I decided I would go further and write my onw assembler for this and it is a simple 2 pass assembler with a static listing table, yes static because I got a bit lazy, just change the LISTSIZE in LC3assembler.c if you want to support more labels. My initial aim was to make one for RISC V but the ISA is just too long and most of the work is kinda repetitive, so RISC-V implementation will begin early 2026 hopefully and It should feature a proper operating system and like a CLI interface maybe? I dunno I'll see then, till then check this out. This is the VM playing 2048.

![(Docs/image.png)](Docs/image.png)

## Architecture Overview

The LC3 virtual machine implements a complete instruction set architecture emulator using a centralized state structure that holds all CPU state including registers, the current instruction, decoded operands, and execution flags.

### Register Architecture

The LC3 provides 10 registers:
- **R0-R7**: General-purpose registers (R0 is hardwired to zero)
- **PC**: Program Counter (16-bit, initialized to 0x3000)
- **COND**: Condition Code register with three flags:
  - POS (Positive): Set when result > 0
  - ZRO (Zero): Set when result = 0
  - NEG (Negative): Set when result < 0 (bit 15 set)

### Instruction Set

The LC3 implements 16 instruction types:

**Arithmetic and Logic:**
- ADD: Addition (register or immediate)
- AND: Bitwise AND (register or immediate)
- NOT: Bitwise NOT

**Memory Operations:**
- LD: Load from memory using PC-relative addressing
- LDI: Load indirect (double indirection)
- LDR: Load register (base + offset)
- ST: Store to memory using PC-relative addressing
- STI: Store indirect
- STR: Store register (base + offset)
- LEA: Load effective address

**Control Flow:**
- BR: Conditional branch (supports N, Z, P condition flags)
- JMP: Jump to address in register (RET when using R7)
- JSR: Jump to subroutine (PC-relative or register-based)
- RTI: Return from interrupt (unused in this implementation)

**System:**
- TRAP: Execute trap routine (6 routines implemented)
- RES: Reserved opcode (unused)

### Trap Routines

The VM implements 6 trap routines for I/O operations:
- TRAP_GETC (0x20): Get character from keyboard (not echoed)
- TRAP_OUT (0x21): Output character to terminal
- TRAP_PUTS (0x22): Output null-terminated string
- TRAP_IN (0x23): Get character from keyboard (echoed)
- TRAP_PUTSP (0x24): Output byte string (two characters per word)
- TRAP_HALT (0x25): Halt program execution

### Memory System

- **Addressable Space**: 64KB (65,536 bytes, 32,768 words)
- **Word Size**: 16 bits
- **Byte Ordering**: Big-endian (bytes are swapped when reading/writing object files)
- **Memory Layout**:
  - Addresses below 0x3000: Reserved for trap routines
  - 0x3000: Default program start address
  - 0xFE00 (KBSR): Keyboard status register (memory-mapped I/O)
  - 0xFE02 (KBDR): Keyboard data register (memory-mapped I/O)

## How to Run

### Building the Virtual Machine

Navigate to the LC3 directory and compile:

```bash
cd LC3
gcc -o LC3 LC3.c
```

Or use the Makefile:

```bash
cd LC3
make
```

### Running Programs

The virtual machine executes LC3 object files (`.obj` format). You can run a program by passing the object file as an argument:

```bash
./LC3 helloworld.obj
```

You can also load multiple object files:

```bash
./LC3 program1.obj program2.obj
```

### Example Programs

The project includes several example programs:
- `helloworld.obj` - Hello world
- `2048.obj` - 2048 game
- `revstring.obj` - String reversal
- Assembly source files: `charcount.asm`, `mul10.asm`, `population.asm`, `revstring.asm`

### Building and Using the Assembler

To compile:

```bash
cd LC3
gcc -o LC3assembler LC3assembler.c
```

To assemble a program:

```bash
./LC3assembler program.asm
```

This will generate a `.obj` file that can be executed by the virtual machine.

## Project Structure

```
.
├── LC3/                      # Main implementation directory
│   ├── LC3.c                # Virtual machine implementation
│   ├── LC3.h                # Header file with definitions and macros
│   ├── LC3assembler.c       # Assembler implementation (two-pass)
│   ├── LC3assembler.h       # Assembler header with instruction names
│   ├── Makefile             # Build configuration
│   ├── *.obj                # Example object files
│   └── *.asm                # Example assembly source files
├── Docs/                     # Documentation
│   ├── Design Document.md   # Detailed design decisions
│   ├── ISA explanation.md    # LC3 instruction set reference
│   └── image.png            # Screenshots
└── README.md                # This file
```

## References

- [Write your own VM](https://www.jmeiners.com/lc3-vm/) by Justin Meiners and Ryan Pendleton
- [LC3 ISA description](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf)
- [Assembler tutorial](https://www.youtube.com/watch?v=rdKX9hzA2lU)
- [Assemblers and Loaders](https://www.davidsalomon.name/assem.advertis/AssemAd.html)
