#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

int check_extension(const char *filename){
    const char*ext = "asm";
    const char *dot = strrchr(filename,'.');
    if(!dot || dot == filename){return 1;} // starts with dot | no dot in file
    else{ return strcmp(dot+1,ext); } // compare and return
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
    char linebuf[256];
    FILE*file = fopen(argv[1],"r");
    if (file == NULL) {
        perror("Error opening file.\n");
        exit(1);
    }
    while(fgets(linebuf, sizeof(linebuf), file)){
        if(linebuf[0]==';') continue;
        printf(linebuf);
    }
    return 1;
}