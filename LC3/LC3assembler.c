#include"LC3assembler.h"

#define LISTSIZE 64

symbols list[LISTSIZE];

uint16_t LOCTR = 0;


// Symbol Lookup
uint16_t symbol_lookup(char*label){
    label[strcspn(label, "\r\n")] = '\0';
    for(int i = 0; i < LISTSIZE; i++){
        if(!strcmp(label, list[i].name)) return list[i].address;
    }
    return 0xffff;
}

// instr_assemblers

void BR(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0x0000;
    char*cond = strdup(*instr);
    // printf("%s\n",cond);
    if(strchr(cond,'n') != NULL){ *instruction |= 0x0800; }
    if(strchr(cond,'z') != NULL){ *instruction |= 0x0400; }
    if(strchr(cond,'p') != NULL){ *instruction |= 0x0200; }
    *instr = strtok(NULL," ");
    if(*instr == NULL) {
        fprintf(stderr, "Error: Expected label after BR condition\n");
        free(cond);
        return;
    }
    uint16_t label_addr = symbol_lookup(*instr);
    int16_t pc_offset = label_addr - (LOCTR + 1);
    *instruction |= (pc_offset & 0x01FF);
    fwrite(instruction, sizeof(uint16_t),1,out);
    free(cond);
}

void ADD(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0x1000;
    *instr = strtok(NULL,",");
    uint16_t dr = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (dr & 0x0007) << 9;
    *instr = strtok(NULL,",");
    uint16_t sr1 = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (sr1 & 0x0007) << 6;
    *instr = strtok(NULL,",");
    if((*instr)[0] == 'R'){
        uint16_t sr2 = (uint16_t)strtol(*instr + 1, NULL, 10);
        *instruction |= (sr2 & 0x0007);
    }else{
        int16_t imm5 = (int16_t)strtol(*instr, NULL, 10);
        *instruction |= 0x0020;
          *instruction |= (imm5 & 0x001F);
    }
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void LD(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0x2000;
    *instr = strtok(NULL,",");
    uint16_t dr = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (dr & 0x0007) << 9;
    *instr = strtok(NULL,",");
    uint16_t label_addr = symbol_lookup(*instr);
    int16_t pc_offset = label_addr - (LOCTR + 1);
    *instruction |= (pc_offset & 0x01FF);
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void ST(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0x3000;
    *instr = strtok(NULL,",");
    uint16_t sr = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (sr & 0x0007) << 9;
    *instr = strtok(NULL,",");
    uint16_t label_addr = symbol_lookup(*instr);
    int16_t pc_offset = label_addr - (LOCTR + 1);
    *instruction |= (pc_offset & 0x01FF);
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void JSR(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0x4000;
    *instr = strtok(NULL," ");
    uint16_t label_addr = symbol_lookup(*instr);
    if(label_addr!=0xffff){
        int16_t pc_offset = label_addr - (LOCTR + 1);
        *instruction |= (pc_offset & 0x07FF);
        fwrite(instruction, sizeof(uint16_t),1,out);
    }else{
        uint16_t baseR = (uint16_t)strtol(*instr + 1, NULL, 10);
        *instruction |= (baseR & 0x0007) << 6;
        fwrite(instruction, sizeof(uint16_t),1,out);
    }
}
void AND(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0x5000;
    *instr = strtok(NULL,",");
    uint16_t dr = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (dr & 0x0007) << 9;
    *instr = strtok(NULL,",");
    uint16_t sr1 = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (sr1 & 0x0007) << 6;
    *instr = strtok(NULL,",");
    if((*instr)[0] == 'R'){
        uint16_t sr2 = (uint16_t)strtol(*instr + 1, NULL, 10);
        *instruction |= (sr2 & 0x0007);
    }else{
        int16_t imm5 = (int16_t)strtol(*instr, NULL, 10);
        *instruction |= 0x0020;
          *instruction |= (imm5 & 0x001F);
    }
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void LDR(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0x6000;
    *instr = strtok(NULL,",");
    uint16_t dr = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (dr & 0x0007) << 9;
    *instr = strtok(NULL,",");
    uint16_t baseR = (uint16_t)strtol(*instr + 1, NULL, 10);
    *instruction |= (baseR & 0x0007) << 6;
    *instr = strtok(NULL,",");
    int16_t offset6 = (int16_t)strtol(*instr, NULL, 10);
    *instruction |= (offset6 & 0x003F);
    fwrite(instruction, sizeof(uint16_t),1,out);
}

// void STR(char**instr, uint16_t*instruction, FILE*out);
// void RTI(char**instr, uint16_t*instruction, FILE*out);
// void NOT(char**instr, uint16_t*instruction, FILE*out);
// void LDI(char**instr, uint16_t*instruction, FILE*out);
// void STI(char**instr, uint16_t*instruction, FILE*out);
// void JMP(char**instr, uint16_t*instruction, FILE*out);
// void RES(char**instr, uint16_t*instruction, FILE*out);
// void LEA(char**instr, uint16_t*instruction, FILE*out);

void GETC(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0xF020;
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void OUT(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0xF021;
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void PUTS(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0xF022;
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void IN(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0xF023;
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void PUTSP(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0xF024;
    fwrite(instruction, sizeof(uint16_t),1,out);
}

void HALT(char**instr, uint16_t*instruction, FILE*out){
    *instruction |= 0xF025;
    fwrite(instruction, sizeof(uint16_t),1,out);
}

// printing the listing table
void print_list(int symcount){
    for(int i = 0; i<symcount; i++){
        printf("%s ",list[i].name);
        printf("%i\n",list[i].address);
    }
    printf("\n\n");

}

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
    FILE*out = fopen(filename,"w+b");
    if (out == NULL) {
        perror("Error opening obj file.\n");
        exit(2);
    }
    int symcount = 0;
    char linebuf[256];

    printf("=========================\n");
    // First Pass
    while(fgets(linebuf, sizeof(linebuf), file)){
        char*instr = strtok(linebuf," ");
        while(instr!=NULL){

            instr[strcspn(instr,"\n\r")] = '\0';

            if(strlen(instr) == 0) {
                instr = strtok(NULL," ");
                continue;
            }

            // Ignores comments
            if(!strcmp(instr,";;") || !strcmp(instr,";")) {break;}
            
            // Handles directives
            if(instr[0] == '.'){
                // Break if end is reached.
                if(!strcmp(instr,".END") || !strcmp(instr,".end")){break;}
                // Skip the FILL stmt
                if(!strcmp(instr,".FILL") || !strcmp(instr,".fill")){LOCTR++; break;}
                // Origin directive
                if(!strcmp(instr,".ORIG") || !strcmp(instr,".orig")){
                    list[symcount].name = strdup(instr);
                    instr = strtok(NULL, " ");
                    LOCTR = (uint16_t)strtol(instr + 1, NULL, 16);
                    list[symcount].address = LOCTR;
                    symcount++;
                }
                // allocated that many bytes of memory
                if(!strcmp(instr,".BLKW") || !strcmp(instr,".blkw")){
                    instr = strtok(NULL, " ");
                    LOCTR += (uint16_t)strtol(instr + 1, NULL, 16);
                }
                // String directive.
                if(!strcmp(instr,".STRINGZ") || !strcmp(instr,".stringz")){
                    instr = strtok(NULL, " ");
                    if(instr != NULL){
                        char *str = instr;
                        if(str[0] == '"'){
                            str++;
                            str[strcspn(str, "\"")] = '\0';
                        }
                        LOCTR += strlen(str) + 1;
                    }
                    break;
                }
            }
            else if(is_opcode(&instr)){
                LOCTR++; 
                break;
            }
            else{
                list[symcount].name = strdup(instr);
                list[symcount].address = LOCTR;
                symcount++;
            }
            instr = strtok(NULL," ");
        }
    }
    print_list(symcount);  
    rewind(file);
    LOCTR = 0;
    while(fgets(linebuf,sizeof(linebuf),file)){
        char*instr = strtok(linebuf," ");
        uint16_t instruction = 0;
        while(instr!=NULL){
            
            instr[strcspn(instr,"\n\r")] = '\0';
            
            if(!strcmp(instr,";;") || !strcmp(instr,";")) {break;}

            // Handles directives
            if(instr[0] == '.'){
                // Break if end is reached.
                if(!strcmp(instr,".END") || !strcmp(instr,".end")){break;}
                // Skip the FILL stmt
                if(!strcmp(instr,".FILL") || !strcmp(instr,".fill")){break;}
                // Origin directive
                if(!strcmp(instr,".ORIG") || !strcmp(instr,".orig")){
                    instr = strtok(NULL, " ");
                    instruction = (uint16_t)strtol(instr + 1, NULL, 16);
                    LOCTR = instruction;
                    printf("%X\n",instruction);
                    fwrite(&instruction, sizeof(uint16_t),1,out);
                }
                // allocated that many bytes of memory
                if(!strcmp(instr,".BLKW") || !strcmp(instr,".blkw")){
                }
                // String directive.
                if(!strcmp(instr,".STRINGZ") || !strcmp(instr,".stringz")){
                }
            }

            if(is_opcode(&instr)){
                if(!strcmp(instr, "ADD")){
                    ADD(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "BR")){
                    BR(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "LD")){
                    LD(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "ST")){
                    ST(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "JSR")){
                    JSR(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "AND")){
                    AND(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "GETC")){
                    GETC(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "PUTS")){
                    PUTS(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "OUT")){
                    OUT(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "IN")){
                    IN(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "PUTSP")){
                    PUTSP(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "LDR")){
                    LDR(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                if(!strcmp(instr, "HALT")){
                    HALT(&instr, &instruction, out);
                    printf("%X: 0x%04X\n",LOCTR, instruction);
                    instruction = 0;
                }
                LOCTR++;
            }
            else{}

            instr = strtok(NULL," ");
            } 
        }
        printf("=========================\n");
        fclose(file);
        fclose(out);
        return 1;
    }

