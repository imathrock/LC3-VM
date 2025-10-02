# Design Document

I decided to follow the LC3 VM tutorial and I would be making an LC3 virtual machine first and then move on to RV32I. 
## LC3

### Registers
There are a total of 10 registers, 8 general purpose, 1 program counter, 1 condition register used for branching. 
The instruction set of LC3 does not support branching based on register values like SM213 designed by mike feely. This ISA feels simple enough that I might try and make a VM for that aswell after LC3. 

## Registers
There are a total of 32 registers, they are denoted as follows:
- x0-31
- PC
There is no defined stack pointer register, No defined base pointer register. So we will choose the registers for these purposes from the calling conventions. 

## Calling Conventions

## Memory
I have defined the total memory for this CPU to be around 2 Mb. The initial addresses will serve as instruction memory. The first half of the remaining would be the heap and the rest the stack. 