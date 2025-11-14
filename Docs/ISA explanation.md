# LC3 Instruction Set Architecture Reference

This document provides a reference for the LC3 instruction set architecture implemented in this virtual machine.

## LC3 Instruction Set Overview

The Little Computer 3 (LC3) is a 16-bit instruction set architecture designed for educational purposes. All instructions are 16 bits wide, with a 4-bit opcode (bits 15-12) and varying operand formats.

For complete and authoritative documentation, refer to the [LC3 ISA Specification](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf). This document provides a brief overview of the instruction set.

## Instruction Categories

### Arithmetic and Logic Instructions

**ADD - Addition**
- Format: `ADD DR, SR1, SR2` or `ADD DR, SR1, imm5`
- Opcode: 0x1
- Description: Adds two values and stores result in destination register. Supports register-register or register-immediate modes (bit 5 selects mode).
- Condition codes: Set based on result

**AND - Bitwise AND**
- Format: `AND DR, SR1, SR2` or `AND DR, SR1, imm5`
- Opcode: 0x5
- Description: Performs bitwise AND operation. Supports register-register or register-immediate modes.
- Condition codes: Set based on result

**NOT - Bitwise NOT**
- Format: `NOT DR, SR`
- Opcode: 0x9
- Description: Performs bitwise NOT (one's complement) operation.
- Condition codes: Set based on result

### Memory Instructions

**LD - Load**
- Format: `LD DR, label`
- Opcode: 0x2
- Description: Loads value from memory address (PC + 9-bit signed offset) into destination register.
- Condition codes: Set based on loaded value

**LDI - Load Indirect**
- Format: `LDI DR, label`
- Opcode: 0xA
- Description: Loads address from (PC + 9-bit signed offset), then loads value from that address (double indirection).
- Condition codes: Set based on loaded value

**LDR - Load Register**
- Format: `LDR DR, BaseR, offset6`
- Opcode: 0x6
- Description: Loads value from memory address (BaseR + 6-bit signed offset) into destination register.
- Condition codes: Set based on loaded value

**ST - Store**
- Format: `ST SR, label`
- Opcode: 0x3
- Description: Stores source register value to memory address (PC + 9-bit signed offset).

**STI - Store Indirect**
- Format: `STI SR, label`
- Opcode: 0xB
- Description: Stores source register value to address found at (PC + 9-bit signed offset).

**STR - Store Register**
- Format: `STR SR, BaseR, offset6`
- Opcode: 0x7
- Description: Stores source register value to memory address (BaseR + 6-bit signed offset).

**LEA - Load Effective Address**
- Format: `LEA DR, label`
- Opcode: 0xE
- Description: Computes effective address (PC + 9-bit signed offset) and stores in destination register.
- Condition codes: Set based on computed address

### Control Flow Instructions

**BR - Branch**
- Format: `BRnzp label` (or combinations: BRn, BRz, BRp, BRnz, BRnp, BRzp, BRnzp)
- Opcode: 0x0
- Description: Conditionally branches to (PC + 9-bit signed offset) if condition flags match. Bits 11-9 encode condition (N, Z, P).

**JMP - Jump**
- Format: `JMP BaseR` or `RET` (when BaseR is R7)
- Opcode: 0xC
- Description: Unconditionally jumps to address in BaseR register. RET is a special case using R7.

**JSR - Jump to Subroutine**
- Format: `JSR label` or `JSRR BaseR`
- Opcode: 0x4
- Description: Saves PC in R7, then jumps to subroutine. Supports PC-relative (bit 11 set) or register-based modes.

**RTI - Return from Interrupt**
- Format: `RTI`
- Opcode: 0x8
- Description: Returns from interrupt service routine. Unused in this implementation.

### System Instructions

**TRAP - Execute Trap Routine**
- Format: `TRAP trapvector8`
- Opcode: 0xF
- Description: Executes system routine specified by 8-bit trap vector. Saves PC in R7 before jumping.

**RES - Reserved**
- Opcode: 0xD
- Description: Reserved opcode, unused in this implementation.

## Trap Routines

The LC3 provides several built-in trap routines for I/O operations:

**TRAP_GETC (0x20)**
- Gets character from keyboard without echoing to terminal
- Stores character in R0
- Sets condition codes based on character value

**TRAP_OUT (0x21)**
- Outputs character from R0 to terminal

**TRAP_PUTS (0x22)**
- Outputs null-terminated string starting at address in R0
- String is stored as sequence of 16-bit words, one character per word

**TRAP_IN (0x23)**
- Gets character from keyboard and echoes to terminal
- Prompts user with "Enter a character: "
- Stores character in R0
- Sets condition codes based on character value

**TRAP_PUTSP (0x24)**
- Outputs byte string starting at address in R0
- Each 16-bit word contains two characters (low byte, high byte)
- Continues until null byte is encountered

**TRAP_HALT (0x25)**
- Halts program execution
- Prints "HALT" message and exits

## Instruction Encoding

All LC3 instructions are 16 bits with the following general format:

```
Bits 15-12: Opcode
Bits 11-0:  Operands (format varies by instruction)
```

Common operand formats:
- **Register fields**: 3 bits each (bits 11-9 for DR, bits 8-6 for SR1/BaseR, bits 2-0 for SR2)
- **Immediate values**: 5 bits (bits 4-0) with sign extension
- **PC offsets**: 9 bits (bits 8-0) or 11 bits (bits 10-0) with sign extension
- **Trap vector**: 8 bits (bits 7-0)

## Condition Codes

The condition code register (COND) stores the result of the last operation that sets condition codes:
- **POS (Positive)**: Result is greater than zero
- **ZRO (Zero)**: Result equals zero
- **NEG (Negative)**: Result is less than zero (MSB is 1)

Condition codes are set by: ADD, AND, NOT, LD, LDI, LDR, LEA, and trap routines that modify R0.

## Memory Model

- **Addressable Space**: 64KB (65,536 bytes, 32,768 words)
- **Word Size**: 16 bits
- **Byte Ordering**: Big-endian in object files
- **Memory-Mapped I/O**:
  - 0xFE00 (KBSR): Keyboard status register
  - 0xFE02 (KBDR): Keyboard data register

## Future Plans: RISC-V RV32I Instructions

(To be documented when RISC-V implementation begins)
