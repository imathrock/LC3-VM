#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include<stdio.h>
#include<stdint.h>

#define max_mem (1<<24) // total number of addresses

static int memory[max_mem]; // Total 2 Mb of memory for this computer

/// @brief GPT generated enum of all the registers. Makes it easier to write instructions. 
enum {
    X0,  /* zero  - hardwired 0 */
    X1,  /* ra    - return address */
    X2,  /* sp    - stack pointer */
    X3,  /* gp    - global pointer */
    X4,  /* tp    - thread pointer */
    X5,  /* t0    - temporary / alternate link register */
    X6,  /* t1    - temporary */
    X7,  /* t2    - temporary */
    X8,  /* s0/fp - saved register / frame pointer */
    X9,  /* s1    - saved register */
    X10, /* a0    - function arg / return value */
    X11, /* a1    - function arg / return value */
    X12, /* a2    - function arg */
    X13, /* a3    - function arg */
    X14, /* a4    - function arg */
    X15, /* a5    - function arg */
    X16, /* a6    - function arg */
    X17, /* a7    - function arg */
    X18, /* s2    - saved register */
    X19, /* s3    - saved register */
    X20, /* s4    - saved register */
    X21, /* s5    - saved register */
    X22, /* s6    - saved register */
    X23, /* s7    - saved register */
    X24, /* s8    - saved register */
    X25, /* s9    - saved register */
    X26, /* s10   - saved register */
    X27, /* s11   - saved register */
    X28, /* t3    - temporary */
    X29, /* t4    - temporary */
    X30, /* t5    - temporary */
    X31, /* t6    - temporary */

    REG_COUNT
};


/// @brief This is the Whole instruction set to be implemented. Enum generated using GPT, menial.
typedef enum {
    /* Integer register–immediate */
    ADDI, SLTI, SLTIU, XORI, ORI, ANDI,
    SLLI, SRLI, SRAI,
    /* Integer register–register */
    ADD, SUB, SLL, SLT, SLTU, XOR_, SRL, SRA, OR, AND_,
    /* Loads */
    LB, LH, LW, LBU, LHU,
    /* Stores */
    SB, SH, SW,
    /* Control transfer */
    JAL, JALR,
    BEQ, BNE, BLT, BGE, BLTU, BGEU,
    /* Upper immediate */
    LUI, AUIPC,
    /* System */
    ECALL, EBREAK,
    CSRRW, CSRRS, CSRRC,
    CSRRWI, CSRRSI, CSRRCI,

    INSTR_COUNT
} RiscVInstr;


/// @brief CPU state representation struct, all the CPU current metadata is stored in this
typedef struct {
    int registers[REG_COUNT];
    uint32_t PC; // 32 bit unsigned to allow for a larger range of values.
}RVSM;



#endif