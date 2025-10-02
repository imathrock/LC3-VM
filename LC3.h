#include <stdio.h>
#include <stdint.h>

/*
  LC3 has a maximum of 65536 bytes of memory, 2^16 bits, Which means this is a total of 128 kilobytes.
  Kinda simulates a microcontroller amount of memory.
*/
#define max_mem_size (1<<16)
uint16_t memory[max_mem_size];

/*
    Registers:
    Any kind of CPU has registers that act as a "workbench" for the CPU to do operations on. The Idea of a CPU is 
    that it loads memory into a register, modifies it using some programmed operation and stores it back into either
    another register or back into memory. In embedded systems you can assign a register to a GPIO and by setting the
    values of the Register we can control the outputs. In the following code I define all of the 10 registers in LC3.
    The R0 register is hardwired to zero. There are 7 general purpose registers and 
*/
enum {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    PC,
    COND,
    COUNT // total number of registers
};
/// @brief Using an array to define the registers
uint16_t reg[COUNT];

/*
    Instruction Set:
    The LC3 computer has a total of 16 instructions, Just like SimpleMachine213 that was designed for UBC CPSC 213. 
    The difference here is that the branching instructions do not work based on register value. 
    I noticed that there was no halt instruction so I have added it in here and might remove it later on if it DNE.
*/
enum{
    BR = 0, // Unconditional branch, just like jump but can only change PC by a small number
    ADD,    // Addition
    LD,     // loading memory into registers
    ST,     // Storing memory from registers
    JSR,    // jump register
    AND,    // Bitwise AND
    LDR,    // Load register
    STR,    // Store register
    RTI,    // unused
    NOT,    // bitwise NOT
    LDI,    // load indirect
    STI,    // Store indirect
    JMP,    // Jump
    RES,    // reserve, Unused (might assign sub instruction to this)
    LEA,    // Load effective address
    TRAP,   // Execute effective trap
    HALT,   // Halts the Processor, This does nto
};

/*
    Condition flags:
    As previously mentioned LC3 uses 3 condition flags to denote if the previous operation resulted in a 
    Positive, Zero or Negative number. This is similar to how the toy ISA y86 works. 
*/
enum{
    POS = 1 << 0,
    ZRO = 1 << 1,
    NEG = 1 << 2
};

/*
    In the tutorial there is no kind of struct that exists to denote the current state of the CPU.
    So I am going to create it here. It should basically present the current state of all registers and cond flags.
*/
typedef struct{
    uint16_t reg[COUNT];
    int COND;
} LC3;