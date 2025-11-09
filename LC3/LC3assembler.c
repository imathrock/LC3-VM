#include"LC3assembler.h"

#define LISTSIZE 64

symbols list[LISTSIZE];

uint16_t LOCTR = 0;

void op(char*instr,FILE*file, void (*opr)(char*,FILE*)){
    opr(instr,file);
}

void handle_dot_directive(char **instr, int *symcount) {
    char *arg = strtok(NULL, " ");
    if (arg == NULL) return;
    LOCTR = (uint16_t)strtol(arg + 1, NULL, 16);
    printf("%i\n", LOCTR);
    list[*symcount].name = arg;
    list[*symcount].address = LOCTR;
    (*symcount)++;
    *instr = arg;
}

void handle_symbol(char **instr, int *symcount){

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

    

    while(fgets(linebuf, sizeof(linebuf), file)){
        char*instr = strtok(linebuf," ");
        while(instr!=NULL){
            // Ignores comments
            if(!strcmp(instr,";;") || !strcmp(instr,";")) {break;}

            if(instr[0] == '.'){
                handle_dot_directive(&instr, &symcount);
            }
            else if(is_opcode(&instr)){LOCTR++;}
            else{ 
                handle_symbol(&instr, &symcount);
                char *next = strtok(NULL, " \t\n");
                if (next) {
                    if (next[0] == '.') handle_dot_directive(&next,&symcount);
                    else if (is_opcode(&next)) LOCTR ++;
                }
            }
            printf("%s\n",instr);
            instr = strtok(NULL, " ");
        }
    }  
    
    rewind(file);
    printf("\n\n\n\n");
    while(fgets(linebuf, sizeof(linebuf), file)){
        uint16_t instruction = 0;
        char*instr = strtok(linebuf," ");
        while(instr!=NULL){
            // Ignores comments
            if(!strcmp(instr,";;") || !strcmp(instr,";")) {break;}

            if(instr[0] == '.'){
                handle_dot_directive(&instr, &symcount);
            }
            else if(is_opcode(&instr)){LOCTR++;}
            else{ 
                handle_symbol(&instr, &symcount);
                // char *next = strtok(NULL, " \t\n");
                // if (next) {
                //     if (next[0] == '.') handle_dot_directive(&next,&symcount);
                //     else if (is_opcode(&next)) LOCTR ++;
                // }
            }
            printf("%s\n",instr);
            instr = strtok(NULL, " ");
        }
    } 
    return 1;
}

// if(!strcmp(instr,";;")) {break;}
// if(!strcmp(instr,";")) {break;}
// op(instr,out,origin_handler);
// if(!strcmp(instr,"BR")) {instruction |= 0x0000;}
// else if(!strcmp(instr,"BRn")) {instruction |= 0x0800;}
// else if(!strcmp(instr,"BRz")) {instruction |= 0x0400;}
// else if(!strcmp(instr,"BRp")) {instruction |= 0x0200;}
// else if(!strcmp(instr,"BRnz")) {instruction |= 0x0C00;}
// else if(!strcmp(instr,"BRnp")) {instruction |= 0x0A00;}
// else if(!strcmp(instr,"BRzp")) {instruction |= 0x0600;}
// else if(!strcmp(instr,"BRnzp")) {instruction |= 0x0E00;}
// else if(!strcmp(instr,"ADD")) {instruction |= 0x1000;}
// else if(!strcmp(instr,"LD")) {instruction |= 0x2000;}
// else if(!strcmp(instr,"ST")) {instruction |= 0x3000;}
// else if(!strcmp(instr,"JSR")) {instruction |= 0x4000;}
// else if(!strcmp(instr,"AND")) {instruction |= 0x5000;}
// else if(!strcmp(instr,"LDR")) {instruction |= 0x6000;}
// else if(!strcmp(instr,"STR")) {instruction |= 0x7000;}
// else if(!strcmp(instr,"RTI")) {instruction |= 0x8000;}
// else if(!strcmp(instr,"NOT")) {instruction |= 0x9000;}
// else if(!strcmp(instr,"LDI")) {instruction |= 0xA000;}
// else if(!strcmp(instr,"STI")) {instruction |= 0xB000;}
// else if(!strcmp(instr,"JMP")) {instruction |= 0xC000;}
// else if(!strcmp(instr,"LEA")) {instruction |= 0xE000;}
// else if(!strcmp(instr,"RES")) {instruction |= 0xD000;}
// else if(!strcmp(instr,"GETC")) {instruction |= 0xF020;}
// else if(!strcmp(instr,"OUT")) {instruction |= 0xF021;}
// else if(!strcmp(instr,"PUTS")) {instruction |= 0xF022;}
// else if(!strcmp(instr,"IN")) {instruction |= 0xF023;}
// else if(!strcmp(instr,"PUTSP")) {instruction |= 0xF024;}
// else if(!strcmp(instr,"HALT")) {instruction |= 0xF025;}
// else{}
// printf("%s\n",instr);