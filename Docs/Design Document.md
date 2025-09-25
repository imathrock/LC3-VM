# Design Document

## Registers
There are a total of 32 registers, they are denoted as follows:
- x0-31
- PC
There is no defined stack pointer register, No defined base pointer register. So we will choose the registers for these purposes from the calling conventions. 

## Calling Conventions

## Memory
I have defined the total memory for this CPU to be around 2 Mb. The initial addresses will serve as instruction memory. The first half of the remaining would be the heap and the rest the stack. 