#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

/* String names for each instruction enum above. The order MUST match the enum
   so `INSTR_NAMES[INSTR_BR]` -> "BR". Keep these uppercase to match typical
   LC-3 assembly tokens. */
static const char *INSTR_NAMES[] = {
    "BR", "BRn", "BRz", "BRp", "BRnz", "BRnp", "BRzp", "BRnzp",
    "ADD", "LD", "ST", "JSR", "AND", "LDR", "STR", "RTI",
    "NOT", "LDI", "STI", "JMP", "RES", "LEA",
    "GETC", "OUT", "PUTS", "IN", "PUTSP", "HALT"
};

#define INSTR_COUNT (sizeof(INSTR_NAMES)/sizeof(INSTR_NAMES[0]))

typedef struct{
    char*name;
    uint16_t address;
}symbols;

// checks if something is an opcode
int is_opcode(char**instr){
    for(int i = 0; i<INSTR_COUNT; i++){ if(!strcmp(*instr,INSTR_NAMES[i])) return 1; }
    return 0;
}