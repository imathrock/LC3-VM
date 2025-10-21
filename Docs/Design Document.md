# Design Document

I decided to follow the LC3 VM tutorial and I would be making an LC3 virtual machine first and then move on to RV32I. 
## LC3

### Registers
There are a total of 10 registers, 8 general purpose, 1 program counter, 1 condition register used for branching. 
The instruction set of LC3 does not support branching based on register values like SM213 designed by mike feely. This ISA feels simple enough that I might try and make a VM for that aswell after LC3. 

### State machine
I defined a new struct that helps me hold the deconstructed instruction. I defined almost everything i need in a single struct as follows:
```C
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
```
It holds everything I need, I took this Idea form UBC CPSC 313 course where there was an assignment kinda similar to this one where we had to implement the y86 CPU.

This does however make stuff kinda annoying because I have to keep typing "lc3.stuff" again and again because it is statically defined. But I'll make do with it. 

### Instructions:
All of the instructions are well described here [LC3 ISA description](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf)

## Assembler 
I would also like to make an assembler for LC3 to understand how compilers work. Based on my research (a simple ChatGPT summary and Uwaterloo MIPS assembly slides) Assemblers work on a 2 pass system, where in one pass they find all instructions and in another pass they identify other assembly directives and create a hexdump that gets written into the instruction memory of the computer. 

My Idea is to start with a single pass system, I have a C file that takes in address as argument, opens the .asm file, parses thru line by line, tokenizes it, finds the first word, hashes it as an instruction and creates a 16 bit integer based on that. I dont know how the second pass works like how the assembler directives are written into the file but I'll read about it and figure it out. 

## RISC-V 32I

### Registers
There are a total of 32 registers, they are denoted as follows:
- x0-31
- PC
There is no defined stack pointer register, No defined base pointer register. So we will choose the registers for these purposes from the calling conventions. 

### Calling Conventions

### Memory
I have defined the total memory for this CPU to be around 2 Mb. The initial addresses will serve as instruction memory. The first half of the remaining would be the heap and the rest the stack. 