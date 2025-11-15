# LC3 Virtual Machine Project

LC3 virtual machine and assembler implementation. RISC-V implementation will begin early 2026.

## Running the Virtual Machine

Build:
```bash
cd LC3
gcc -o LC3 LC3.c
```

Run an object file:
```bash
./LC3 helloworld.obj
```

Run multiple object files:
```bash
./LC3 program1.obj program2.obj
```

Example programs:
- `helloworld.obj` - Hello world
- `2048.obj` - 2048 game
- `revstring.obj` - String reversal

## Using the Assembler

Build:
```bash
cd LC3
gcc -o LC3assembler LC3assembler.c
```

Assemble a program:
```bash
./LC3assembler program.asm
```

This generates a `.obj` file that can be executed by the virtual machine.

## Project Structure

```
LC3/              # Implementation
  ├── LC3.c       # Virtual machine
  ├── LC3.h       # Header
  ├── LC3assembler.c
  └── *.obj, *.asm
Docs/             # Documentation
  └── Design Document.md
```
