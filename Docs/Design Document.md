# Design Document

This document describes the design decisions and implementation details of the LC3 virtual machine. The implementation follows the LC3 VM tutorial while incorporating design patterns from educational CPU implementations like the y86 from UBC CPSC 313.

## LC3 Virtual Machine

### Register Architecture

The LC3 provides 10 registers total:
- **8 General-Purpose Registers (R0-R7)**: R0 is hardwired to zero and cannot be modified. The remaining 7 registers are available for general computation.
- **Program Counter (PC)**: 16-bit register that points to the current instruction. Initialized to 0x3000, which is the standard LC3 program start address.
- **Condition Code Register (COND)**: Stores the result of the last operation that sets condition codes. Uses three flags:
  - POS (bit 0): Set when result is positive
  - ZRO (bit 1): Set when result is zero
  - NEG (bit 2): Set when result is negative (MSB is 1)

The instruction set of LC3 does not support branching based on register values like SM213 designed by Mike Feeley. This ISA feels simple enough that I might try and make a VM for that as well after LC3.

### State Machine Design

I defined a new struct that helps me hold the deconstructed instruction. I defined almost everything I need in a single struct as follows:

```C
typedef struct{
    uint16_t reg[COUNT];
    int running;
    uint16_t cond;
    uint16_t PC;
    uint16_t instr;
    uint16_t op;
    uint8_t dr, sr1, sr2;
    uint16_t imm_val;
    uint16_t pc_offset;
} LC3;
```

It holds everything I need. I took this idea from UBC CPSC 313 course where there was an assignment kinda similar to this one where we had to implement the y86 CPU.

This does however make stuff kinda annoying because I have to keep typing "cpu.stuff" again and again because it is statically defined. But I'll make do with it.

The struct stores:
- All register values in an array
- Execution state (running flag)
- Current instruction and decoded opcode
- Decoded instruction fields (destination register, source registers, immediate values, PC offsets)
- Condition code register

This centralized approach simplifies instruction execution since all state is accessible through a single structure.

### Instruction Set

The LC3 computer has a total of 16 instructions. The instruction encoding uses a 4-bit opcode (bits 15-12) with varying operand formats:

- **BR (0x0)**: Conditional branch with 9-bit signed PC offset. Supports N, Z, P condition flags.
- **ADD (0x1)**: Addition with two modes - register-register or register-immediate (bit 5 selects mode)
- **LD (0x2)**: Load from memory using PC-relative addressing (9-bit signed offset)
- **ST (0x3)**: Store to memory using PC-relative addressing
- **JSR (0x4)**: Jump to subroutine. Supports PC-relative (bit 11 set) or register-based modes
- **AND (0x5)**: Bitwise AND with register-register or register-immediate modes
- **LDR (0x6)**: Load register using base register + 6-bit signed offset
- **STR (0x7)**: Store register using base register + 6-bit signed offset
- **RTI (0x8)**: Return from interrupt (unused in this implementation)
- **NOT (0x9)**: Bitwise NOT (one's complement)
- **LDI (0xA)**: Load indirect - loads address from PC-relative location, then loads value from that address
- **STI (0xB)**: Store indirect - stores value to address found at PC-relative location
- **JMP (0xC)**: Jump to address in register (RET when using R7)
- **RES (0xD)**: Reserved opcode (unused)
- **LEA (0xE)**: Load effective address - computes PC-relative address and stores in register
- **TRAP (0xF)**: Execute trap routine (8-bit trap vector)

All instructions are well described in the [LC3 ISA description](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf).

### Instruction Decoding

Instruction decoding uses bit manipulation macros for extracting fields:

- `get_op(val)`: Extracts opcode from bits 15-12
- `dr(val)`: Extracts destination register from bits 11-9
- `sr1(val)`: Extracts first source register from bits 8-6
- `sr2(val)`: Extracts second source register from bits 2-0
- `baseR(val)`: Extracts base register from bits 8-6
- `pc_offset(val)`: Extracts 9-bit signed PC offset with sign extension
- `pcoffset11(val)`: Extracts 11-bit signed PC offset with sign extension
- `imm5(val)`: Extracts 5-bit signed immediate with sign extension
- `offset6(val)`: Extracts 6-bit signed offset with sign extension
- `trapvec(val)`: Extracts 8-bit trap vector
- `cond(val)`: Extracts condition flags from bits 11-9

The `SIGN_EXTEND` macro handles sign extension for immediate values and offsets, which is critical for proper two's complement arithmetic.

### Memory System

LC3 has a maximum of 65536 bytes of memory (2^16), which means this is a total of 128 kilobytes. This kinda simulates a microcontroller amount of memory.

Memory is implemented as a global array of 16-bit words:
```C
#define max_mem_size (1<<16)
uint16_t memory[max_mem_size];
```

Memory access functions:
- `MEM_READ(address)`: Reads from memory, with special handling for memory-mapped I/O (KBSR)
- `MEM_WRITE(address, value)`: Writes to memory with bounds checking

**Memory Layout:**
- Addresses 0x0000 - 0x2FFF: Reserved for trap routines and system use
- Address 0x3000: Default program start address
- Address 0xFE00 (KBSR): Keyboard status register (bit 15 indicates key ready)
- Address 0xFE02 (KBDR): Keyboard data register (contains pressed key)

### Input/Output Handling

The implementation uses Windows-specific APIs for real-time keyboard input:

- `disable_input_buffering()`: Configures console to disable echo and line input, allowing character-by-character input
- `check_key()`: Non-blocking keyboard check using `_kbhit()` from `<conio.h>`
- `restore_input_buffering()`: Restores original console mode on program exit

Memory-mapped I/O is implemented in `MEM_READ()`:
- When reading from KBSR (0xFE00), the function checks for keyboard input
- If a key is pressed, KBSR bit 15 is set and the character is stored in KBDR (0xFE02)
- If no key is pressed, KBSR is cleared

### Condition Code Setting

The `setcc()` function updates condition codes based on register value:
- If register is zero: Set ZRO flag
- If register MSB (bit 15) is set: Set NEG flag
- Otherwise: Set POS flag

Condition codes are set after operations that modify registers (ADD, AND, NOT, LD, LDI, LDR, LEA, and trap routines that modify R0).

### Object File Format

LC3 object files use a simple binary format:
1. First 16-bit word: Origin address (where to load the program) in big-endian format
2. Subsequent 16-bit words: Program instructions/data in big-endian format

The VM performs byte swapping when reading object files using `swap16()` to handle endianness conversion between the file format and the host system.

## Assembler Implementation

I would also like to make an assembler for LC3 to understand how compilers work. Based on my research (a simple ChatGPT summary and Uwaterloo MIPS assembly slides) assemblers work on a 2 pass system, where in one pass they find all instructions and in another pass they identify other assembly directives and create a hexdump that gets written into the instruction memory of the computer.

My idea is to start with a single pass system, I have a C file that takes in address as argument, opens the .asm file, parses through line by line, tokenizes it, finds the first word, hashes it as an instruction and creates a 16 bit integer based on that. I don't know how the second pass works like how the assembler directives are written into the file but I'll read about it and figure it out.

### Current Implementation

The assembler is implementing a two-pass design:

**First Pass:**
- Scans assembly source file line by line
- Builds symbol table by identifying labels and directives
- Calculates addresses for all symbols using location counter (LOCTR)
- Handles assembly directives:
  - `.ORIG`: Sets program origin address
  - `.FILL`: Allocates one word with specified value
  - `.BLKW`: Allocates block of words
  - `.STRINGZ`: Allocates null-terminated string
  - `.END`: Marks end of assembly

**Second Pass:**
- Generates instruction encodings using symbol table
- Resolves forward references to labels
- Outputs object file in LC3 binary format

The assembler supports:
- All 16 LC3 instructions with proper encoding
- Label definitions and references
- Assembly directives
- Comments (lines starting with `;` or `;;`)

Symbol table is implemented as an array of structures containing label names and addresses. The current implementation supports up to 64 symbols (LISTSIZE).

## Future Plans: RISC-V 32I

### Registers

There are a total of 32 registers, they are denoted as follows:
- x0-31: General-purpose registers
- PC: Program Counter

There is no defined stack pointer register, no defined base pointer register. So we will choose the registers for these purposes from the calling conventions.

### Calling Conventions

(To be implemented)

### Memory

I have defined the total memory for this CPU to be around 2 MB. The initial addresses will serve as instruction memory. The first half of the remaining would be the heap and the rest the stack.

