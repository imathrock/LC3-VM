# Project Management

This file tracks objectives and tasks for the LC3 Virtual Machine project. Updated as work progresses.

## Objectives

- Implement a complete LC3 ISA virtual machine that can execute LC3 programs
- Build a two-pass assembler for converting LC3 assembly to object files
- Understand computer architecture, instruction set design, and low-level systems programming
- Create a state machine architecture that tracks CPU state throughout execution
- Support reading instructions from object files and emulating memory (heap and stack)
- Implement memory-mapped I/O for interactive programs

## Completed Tasks

- [X] Create register array and enums that define all of the registers
- [X] Create enums for all instructions
- [X] Start following the LC3 virtual machine tutorial, understand and define the architecture
- [X] Implement all 16 LC3 instructions
- [X] Create LC3 state machine with centralized state structure
- [X] Fix the implementation by reimplementing
- [X] Implement memory-mapped I/O for keyboard input
- [X] Implement all 6 trap routines (GETC, OUT, PUTS, IN, PUTSP, HALT)
- [X] Implement object file loading with byte swapping
- [X] Create first pass of assembler (symbol table building)
- [X] Implement assembly directive parsing (.ORIG, .FILL, .BLKW, .STRINGZ, .END)

## In Progress

- [ ] Complete second pass of assembler (instruction encoding and forward reference resolution)
- [ ] Test assembler with all example programs
- [ ] Add support for additional assembly directives if needed

## Future Tasks

- [ ] Write comprehensive test suite for virtual machine
- [ ] Add debugging features (instruction tracing, register dump, memory inspection)
- [ ] Optimize instruction execution if needed
- [ ] Document trap routine implementation details
- [ ] Create more example programs
- [ ] Begin RISC-V RV32I virtual machine implementation
