#include"LC3assembler.h"

#define LISTSIZE 64

symbols list[LISTSIZE];

uint16_t LOCTR = 0;

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

            // Ignores comments
            if(!strcmp(instr,";;") || !strcmp(instr,";")) {break;}
            if(strlen(instr) == 0) {
                instr = strtok(NULL," ");
                continue;
            }
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
    return 1;
}
