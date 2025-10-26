#include <stdio.h>
#include <stdint.h>

#include <signal.h>
/* windows only */
#include <Windows.h>
#include <conio.h>  // _kbhit

HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;

void disable_input_buffering()
{
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwOldMode); /* save old mode */
    fdwMode = fdwOldMode
            ^ ENABLE_ECHO_INPUT  /* no input echo */
            ^ ENABLE_LINE_INPUT; /* return when one or
                                    more characters are available */
    SetConsoleMode(hStdin, fdwMode); /* set new mode */
    FlushConsoleInputBuffer(hStdin); /* clear buffer */
}

void restore_input_buffering()
{
    SetConsoleMode(hStdin, fdwOldMode);
}

uint16_t check_key()
{
    return WaitForSingleObject(hStdin, 0) == WAIT_OBJECT_0 && _kbhit();
}

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
    COUNT // total number of registers
};

/*
    Instruction Set:
    The LC3 computer has a total of 16 instructions, Just like SimpleMachine213 that was designed for UBC CPSC 213. 
    The difference here is that the branching instructions do not work based on register value. 
    I noticed that there was no halt instruction so I have added it in here and might remove it later on if it DNE.
*/
enum{
    BR  = 0x0, // Unconditional branch, just like jump but can only change PC by a small number
    ADD = 0x1,    // Addition
    LD  = 0x2,     // loading memory into registers
    ST  = 0x3,     // Storing memory from registers
    JSR = 0x4,    // jump register
    AND = 0x5,    // Bitwise AND
    LDR = 0x6,    // Load register
    STR = 0x7,    // Store register
    RTI = 0x8,    // unused
    NOT = 0x9,    // bitwise NOT
    LDI = 0xa,    // load indirect
    STI = 0xb,    // Store indirect
    JMP = 0xc,    // Jump
    RES = 0xd,    // reserve, Unused (might assign sub instruction to this)
    LEA = 0xe,    // Load effective address
    TRAP= 0xf   // Execute effective trap
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
    Memory mapped IO, Used for keyboard interrupts
*/
enum{
    KBSR = 0xfe00,
    KBDR = 0xfe02
};

/*
    In the tutorial there is no kind of struct that exists to denote the current state of the CPU.
    So I am going to create it here. It should basically present the current state of all registers and cond flags.
    I am going to maintain this practice for the next ones as well. 
    The reason for this kind of struct is that I can call an instruction deconstructor and store 
    everything required immediately in the struct making it easy to work with. 
*/
typedef struct{
    uint16_t instr;
    uint16_t op;
    uint8_t dr, sr1, sr2;
    uint8_t imm_flag;
    uint16_t imm_val;
    uint16_t pc_offset;
} LC3instr;

typedef struct{
    uint16_t reg[COUNT];
    int running;
    uint16_t cond;
    uint16_t PC;
    LC3instr curr;
} LC3;

#define SIGN_EXTEND(val, bitcount) ((val >> (bitcount-1)) & 1) ? (val | (0xffff << bitcount)) : val

// COMPLETELY UN NECESSARY BUT I JUST WANT IT.
#define get_op(val) (val >> 12)
#define dr(val) (((val) >> 9) & 0x7)
#define sr1(val) (((val) >> 6) & 0x7)
#define sr2(val) ((val) & 0x7)
#define cb(val) ()
#define baseR(val) (((val) >> 6) & 0x7)
#define pc_offset(val) (SIGN_EXTEND((val & 0x1ff),9))
#define pcoffset11(val) (SIGN_EXTEND((val & 0x7ff),11))
#define imm5(val) (SIGN_EXTEND((val) & 0x1f , 5))
#define offset6(val) ((val) & 0x3f)
#define trapvec(val) ((val) & 0xff)
#define cond(val) (((val) >> 9) & 0x7)

void setcc(LC3 *cpu){
    if(cpu->reg[cpu->curr.dr] == 0){cpu->cond = ZRO;}
    if(cpu->reg[cpu->curr.dr] <  0){cpu->cond = NEG;}
    if(cpu->reg[cpu->curr.dr]  > 0){cpu->cond = POS;}
}

/*
    Reading from memory given a Program counter value.
*/
uint16_t MEM_READ(uint16_t addr){
    if(addr == KBSR){
        if(check_key()){
            return memory[KBSR] | 0x8000;  /* Bit 15 is ready */
        } else {
            return 0;
        }
    } else if(addr == KBDR){
        return (uint16_t)getchar();
    }
    return memory[addr];
}

/*
    Writing to memory given address and value and checking bounds to ensure no overflow occurs
*/
int MEM_WRITE(uint16_t addr, uint16_t val){
    if(addr == KBSR || addr == KBDR){
        /* Read-only */
        return 0;
    }
    if(addr < max_mem_size){
        memory[addr] = val;
        return 1;
    }
    return 0;
}