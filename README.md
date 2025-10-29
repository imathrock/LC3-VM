# LC3 Virtual Machine Project

In this project I attempt to learn how virtual machines work and learn more about LC3. My goal is to become proficient in C programming, Learn more about how CPUs work, Learn how compilers work and overall become a better low level developer. 

I will be building a total of 2 virtual machines, when I say virtual machines I mean ISA emulators. There are 2 ISAs that I would try to make VMs for, they are listed as follows:
- Little Computer 3 (Currently working on)
- RISC V

## Current Status
- LC3 Virtual Machine: Implemented
- LC3 Assembler: Currently implementing 1 pass 

## How to Run

### Prerequisites
- GCC compiler
- Windows (current implementation uses Windows-specific headers)

### Building and Running
Navigate to the LC3 directory and use the Makefile:

```bash
cd LC3
make run
```

This will compile the LC3 virtual machine and run it. The VM expects a `.obj` file as input.

### Running Examples

The project includes two example programs:

1. **Hello World Program**
   ```bash
   cd LC3
   gcc -o lc3 LC3.c
   ./lc3 helloworld.obj
   ```

2. **2048 Game**
   ```bash
   cd LC3
   gcc -o lc3 LC3.c
   ./lc3 2048.obj
   ```
Example of the game being played.

![alt text](Docs/image.png)

### Manual Compilation
If you prefer to compile manually:
```bash
cd LC3
gcc -o lc3 LC3.c
./lc3 <program.obj>
```

## Implementation Details

The LC3 virtual machine implements:
- 10 registers (8 general purpose, PC, condition register)
- 16-bit instruction set
- Memory management
- Input/output handling
- State machine architecture

The VM follows the LC3 ISA specification and can execute compiled LC3 programs in `.obj` format.

## References
[Write your own VM by Justin Meiners and Ryan Pendleton](https://www.jmeiners.com/lc3-vm/)

[LC3 ISA description](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf)

[Assembler tutorial](https://www.youtube.com/watch?v=rdKX9hzA2lU)

[Assemblers and Loaders](https://www.davidsalomon.name/assem.advertis/AssemAd.html)