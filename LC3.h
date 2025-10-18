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
    The following is an enum of all of the trap routines. Interesting note, Trap routines are basically function calls
    to pre written subroutines that are written in assembly. In this computer, the addresses below 0x3000 are reserved
    for such trap routines. Whenever these are called they just jump to the routine.
*/
enum{
    TRAP_GETC = 0x20, // gets character from keyboard not echoed onto terminal
    TRAP_OUT = 0x21, // outputs a character
    TRAP_PUTS = 0x22, // Output a word string
    TRAP_IN = 0x23, // Get character from keyboard that is echoed onto terminal
    TRAP_PUTSP = 0x24, // Output a byte string. 
    TRAP_HALT = 0x25 // Halt the program. 
};

/*
    In the tutorial there is no kind of struct that exists to denote the current state of the CPU.
    So I am going to create it here. It should basically present the current state of all registers and cond flags.
    I am going to maintain this practice for the next ones as well. 
    The reason for this kind of struct is that I can call an instruction deconstructor and store 
    everything required immediately in the struct making it easy to work with. 
*/
typedef struct{
    uint16_t reg[COUNT];
    int running;
    uint16_t instr;
    uint16_t op;
    uint8_t dest_reg;
    uint8_t source_reg_1;
    uint8_t source_reg_2;
    uint8_t ival;
    uint16_t pc_offset;
} LC3;

#define SIGN_EXTEND(val, bitcount) ((val >> (bitcount-1)) & 1) ? (val | (0xffff << bitcount)) : val

void update_flag(LC3 lc3){
    if(lc3.reg[lc3.dest_reg] == 0){lc3.reg[COND] = ZRO;}
    else if(lc3.reg[lc3.dest_reg]>>15){lc3.reg[COND] = NEG;}
    else{lc3.reg[COND] = POS;}
}

/*
    Reading from memory given a Program counter value.
*/
#define MEM_READ(PCval) (uint16_t)(memory[PCval])

/*
    Writing to memory given address and value and checking bounds to ensure no overflow occurs
*/
#define MEM_WRITE(addr,val) ((addr) < max_mem_size ? (memory[(addr)] = (uint16_t)(val), 1) : 0)