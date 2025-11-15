# Design Document

Implementation details of the LC3 virtual machine and assembler.

## Virtual Machine Implementation

### State Machine Design

The `LC3` struct centralizes CPU state:
- Register array (R0-R7), PC, condition codes
- Current instruction and decoded fields (opcode, registers, immediates, offsets)
- Running flag for execution control

Instruction fields are decoded immediately after fetch and stored in the struct, simplifying execution logic.

### Execution Loop

Main loop in `main()`:
1. Fetch instruction from `memory[PC]`
2. Decode opcode using `get_op()`
3. Execute via switch statement
4. Update PC and condition codes

Each instruction handler extracts operands using macros (`dr()`, `sr1()`, `pc_offset()`, etc.) and performs the operation. Condition codes are updated via `setcc()` after register-modifying operations.

### Memory System

64KB memory array (`uint16_t memory[max_mem_size]`). Memory-mapped I/O at 0xFE00 (KBSR) and 0xFE02 (KBDR). `MEM_READ()` checks for keyboard input when accessing KBSR. `MEM_WRITE()` includes bounds checking.

### Object File Loading

`read_image()` loads `.obj` files:
- Reads origin address (first word, big-endian)
- Loads instructions/data starting at origin
- Performs byte swapping via `swap16()` for endianness conversion

### Trap Routines

TRAP instruction dispatches to 6 routines:
- GETC/IN: Keyboard input
- OUT/PUTS/PUTSP: Character/string output
- HALT: Stops execution

Windows console input uses `disable_input_buffering()` to enable character-by-character input via `_kbhit()`.

## Assembler Implementation

### Two-Pass Design

**First Pass:**
- Scans source file line-by-line
- Builds symbol table: labels mapped to addresses via location counter (LOCTR)
- Handles directives: `.ORIG` (sets origin), `.FILL`, `.BLKW`, `.STRINGZ` (allocates data), `.END`

**Second Pass:**
- Rewinds file and generates instructions
- Each instruction has an assembler function (e.g., `BR()`, `ADD()`) that:
  - Parses operands using `strtok()`
  - Looks up labels via `symbol_lookup()`
  - Encodes instruction fields
  - Writes big-endian word to output file

### Instruction Encoding

Each instruction function:
- Sets opcode bits
- Extracts register numbers from "R0"-"R7" format
- Calculates PC offsets for labels: `label_addr - (LOCTR + 1)`
- Handles immediate vs register modes (ADD, AND)
- Swaps endianness before writing

Symbol table is a fixed-size array (`LISTSIZE=64`) of `symbols` structs containing name and address.




