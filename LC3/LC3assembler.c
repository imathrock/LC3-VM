#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

enum{
    BR  = 0x0, // Unconditional branch, just like jump but can only change PC by a small number
    ADD = 0x1,    // Addition
    LD  = 0x2,     // loading memory into registers
    ST  = 0x3,     // Storing memory from registers
    JSR = 0x4,    // jump register
    AND = 0x5,    // Bitwise AND
    LDR = 0x6,    // Load register
    STR = 0x7,    // Store register
    RTI = 0x8,    // unused
    NOT = 0x9,    // bitwise NOT
    LDI = 0xa,    // load indirect
    STI = 0xb,    // Store indirect
    JMP = 0xc,    // Jump
    RES = 0xd,    // reserve, Unused (might assign sub instruction to this)
    LEA = 0xe,    // Load effective address
    TRAP= 0xf   // Execute effective trap
};

int check_extension(const char *filename){
    const char*ext = "asm";
    const char *dot = strrchr(filename,'.');
    if(!dot || dot == filename){return 1;} // starts with dot | no dot in file
    else{ return strcmp(dot+1,ext); } // compare and return
}

void change_filetype(char*filename){
    snprintf(filename, 256,"%.*s.obj",(int)(strrchr(filename,'.') - filename), filename);
    printf("Output file name: %s \n",filename);
}

int main(int argc, const char* argv[]) {
    if (argc < 2)   {
        printf("Please provide an assembly file\n");
        exit(2);
    }
    if(check_extension(argv[1]) != 0){
        printf("file type is incorrect");
        exit(2);
    }
    // Changing the type of file
    char filename[strlen(argv[1])+1];
    strcpy(filename,argv[1]);
    change_filetype(&filename[0]);
    
    // opening assembly file
    FILE*file = fopen(argv[1],"r");
    if (file == NULL) {
        perror("Error opening asm file.\n");
        exit(2);
    }
    
    // Creating obj file
    FILE*out = fopen(filename,"w");
    if (out == NULL) {
        perror("Error opening obj file.\n");
        exit(2);
    }
    
    char linebuf[256];
    while(fgets(linebuf, sizeof(linebuf), file)){
        // printf(linebuf);
        uint16_t instruction = 0;
        char*instr = strtok(linebuf," ");
        while(instr!=NULL){
            printf("%s\n",instr);
            // if (strcmp(instr, "BR") == 0) {
            //     instruction |= 0x0000;
            // } else if (strcmp(instr, "ADD") == 0) {
            //     instruction |= 0x1000;
            // } else if (strcmp(instr, "LD") == 0) {
            //     instruction |= 0x2000;
            // } else if (strcmp(instr, "ST") == 0) {
            //     instruction |= 0x3000;
            // } else if (strcmp(instr, "JSR") == 0) {
            //     instruction |= 0x4000;
            // } else if (strcmp(instr, "AND") == 0) {
            //     instruction |= 0x5000;
            // } else if (strcmp(instr, "LDR") == 0) {
            //     instruction |= 0x6000;
            // } else if (strcmp(instr, "STR") == 0) {
            //     instruction |= 0x7000;
            // } else if (strcmp(instr, "RTI") == 0) {
            //     instruction |= 0x8000;
            // } else if (strcmp(instr, "NOT") == 0) {
            //     instruction |= 0x9000;
            // } else if (strcmp(instr, "LDI") == 0) {
            //     instruction |= 0xA000;
            // } else if (strcmp(instr, "STI") == 0) {
            //     instruction |= 0xB000;
            // } else if (strcmp(instr, "JMP") == 0) {
            //     instruction |= 0xC000;
            // } else if (strcmp(instr, "LEA") == 0) {
            //     instruction |= 0xE000;
            // } else if (strcmp(instr, "RES") == 0) {
            //     instruction |= 0xD000;
            // } else if (strcmp(instr, "GETC") == 0) {
            //     instruction |= 0xF020;
            // } else if (strcmp(instr, "OUT") == 0) {
            //     instruction |= 0xF021;
            // } else if (strcmp(instr, "PUTS") == 0) {
            //     instruction |= 0xF022;
            // } else if (strcmp(instr, "IN") == 0) {
            //     instruction |= 0xF023;
            // } else if (strcmp(instr, "PUTSP") == 0) {
            //     instruction |= 0xF024;
            // } else if (strcmp(instr, "HALT") == 0) {
            //     instruction |= 0xF025;
            // } else {
            //     continue;
            // }
            instr = strtok(NULL, " ");
        }
        printf("\n\n");
    }   
    return 1;
}