#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>

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

typedef void (*instr_assmblr)(char**instr, uint16_t*instruction, FILE*out);

void BR(char**instr, uint16_t*instruction, FILE*out);
void ADD(char**instr, uint16_t*instruction, FILE*out);
// void LD(char**instr, uint16_t*instruction, FILE*out);
// void ST(char**instr, uint16_t*instruction, FILE*out);
// void JSR(char**instr, uint16_t*instruction, FILE*out);
// void AND(char**instr, uint16_t*instruction, FILE*out);
// void LDR(char**instr, uint16_t*instruction, FILE*out);
// void STR(char**instr, uint16_t*instruction, FILE*out);
// void RTI(char**instr, uint16_t*instruction, FILE*out);
// void NOT(char**instr, uint16_t*instruction, FILE*out);
// void LDI(char**instr, uint16_t*instruction, FILE*out);
// void STI(char**instr, uint16_t*instruction, FILE*out);
// void JMP(char**instr, uint16_t*instruction, FILE*out);
// void RES(char**instr, uint16_t*instruction, FILE*out);
// void LEA(char**instr, uint16_t*instruction, FILE*out);
// void GETC(char**instr, uint16_t*instruction, FILE*out);
// void OUT(char**instr, uint16_t*instruction, FILE*out);
// void PUTS(char**instr, uint16_t*instruction, FILE*out);
// void IN(char**instr, uint16_t*instruction, FILE*out);
// void PUTSP(char**instr, uint16_t*instruction, FILE*out);
// void HALT(char**instr, uint16_t*instruction, FILE*out);


instr_assmblr INSTR_ASSEMBLERS[] = {
    BR, BR, BR, BR, BR, BR, BR, BR, ADD
    // ADD, LD, ST, JSR, AND, LDR, STR, RTI,
    // NOT, LDI, STI, JMP, RES, LEA,
    // GETC, OUT, PUTS, IN, PUTSP, HALT
};

typedef struct{
    char*name;
    uint16_t address;
}symbols;

// checks if something is an opcode
bool is_opcode(char**instr){
    for(int i = 0; i<INSTR_COUNT; i++){ if(!strcmp(*instr,INSTR_NAMES[i])) return true; }
    return false;
}