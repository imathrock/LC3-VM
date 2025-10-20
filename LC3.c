#include "LC3.h"

static LC3 lc3;

void handle_interrupt(int signal){
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

static uint16_t inline swap16(uint16_t x){
    return (x>>8)|(x<<8);
}

void read_image_file(FILE*file){
    uint16_t origin;
    fread(&origin, sizeof(origin), 1,file);
    origin = swap16(origin);

    uint16_t max_read = max_mem_size - origin;
    uint16_t*p = memory + origin;
    int read = fread(p,sizeof(uint16_t), max_read, file);

    while (read-- > 0){
        *p = swap16(*p);
        ++p;
    }
}

int read_image(const char* image_path)
{
    FILE* file = fopen(image_path, "rb");
    if (!file) { return 0; };
    read_image_file(file);
    fclose(file);
    return 1;
}

int main(int argc, const char* argv[]){
    if (argc < 2)   {
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }
    for (int j = 1; j < argc; ++j){
    if (!read_image(argv[j])){
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }
    lc3.reg[COND] = ZRO;

    lc3.running = 1;
    while(lc3.running){
        lc3.instr = MEM_READ(reg[PC]++); // Increment PC
        lc3.op = lc3.instr >> 12;
        switch (lc3.op)
        {
        case BR:
        /*
            Conditional branch instruction. conditions matched with flags provided
            BRnzp LABEL
        */
            lc3.pc_offset = SIGN_EXTEND(lc3.instr & 0x1ff,9);
            if(lc3.reg[COND] & (uint16_t)((lc3.instr >> 9) &0x7)){ // condition matching with instr.
                lc3.reg[PC] += lc3.pc_offset;
            }
            break;
        case ADD:
        /*
            ADD  DR, SR1, SR2
            ADD  DR, SR1, imm5
            Differentiated between using bit number 5.
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
            if ((lc3.instr>>5)&0x1){
                lc3.ival = SIGN_EXTEND((lc3.instr&0x1f), 5);
                lc3.reg[lc3.dest_reg] = lc3.reg[lc3.source_reg_1] + lc3.ival;
            }else{
                lc3.source_reg_2 = lc3.instr & 0x7;
                lc3.reg[lc3.dest_reg] = lc3.reg[lc3.source_reg_1] + lc3.reg[lc3.source_reg_2];
            }
            update_flag(lc3);
            break;
        case LD:
        /*
            LD  DR, PC_OFFSET
            pcoffset can be a label
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.pc_offset = SIGN_EXTEND((lc3.instr & 0x1ff),9);
            lc3.reg[lc3.dest_reg] = MEM_READ(lc3.reg[PC]+lc3.pc_offset);
            update_flag(lc3);
            break;
        case ST:
        /*
            ST  SR, LABEL
            label is memory location.
        */
            lc3.source_reg_1 = (lc3.instr >> 9) & 0x7;
            lc3.pc_offset = SIGN_EXTEND((lc3.instr & 0x1ff),9);
            MEM_WRITE(lc3.reg[PC] + lc3.pc_offset,lc3.reg[lc3.source_reg_1]);
            break;
        case JSR:
        /* 
            JSR  Label
            JSRR  BaseR
            Stores next PC in R7
        */
            lc3.reg[R7] = lc3.reg[PC];
            if (lc3.instr & 0x0800){ // AND with the relevant bit   
                lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
                lc3.reg[PC] = lc3.reg[lc3.source_reg_1];
            }else{
                lc3.reg[PC] += SIGN_EXTEND((lc3.instr & 0x7ff), 11);
            }
            break;
        case AND:
        /*
            AND  DR, SR1, SR2
            AND  DR, SR1, imm5
            same as ADD
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
            if ((lc3.instr>>5)&0x1){
                lc3.ival = SIGN_EXTEND((lc3.instr&0x1f), 5);
                lc3.reg[lc3.dest_reg] = lc3.reg[lc3.source_reg_1] & lc3.ival;
            }else{
                lc3.source_reg_2 = lc3.instr & 0x7;
                lc3.reg[lc3.dest_reg] = lc3.reg[lc3.source_reg_1] & lc3.reg[lc3.source_reg_2];
            }
            update_flag(lc3);
            break;
        case LDR:
        /*
            LDR  DR, BaseR, offset6
            DR <- BaseR + sign_extend(offset6)
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
            lc3.pc_offset = SIGN_EXTEND((lc3.instr & 0x3f),6);
            lc3.reg[lc3.dest_reg] = MEM_READ(lc3.reg[lc3.source_reg_1] + lc3.pc_offset);
            break;
        case STR:
        /*
            STR  SR, BaseR, offset6
            Store in register addr
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
            lc3.ival = SIGN_EXTEND((lc3.instr&0x1f), 5);
            MEM_WRITE(lc3.reg[lc3.source_reg_1]+lc3.ival,lc3.reg[lc3.dest_reg]);
            break;
        case NOT:
        /*
            NOT  DR, SR
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
            lc3.reg[lc3.dest_reg] = ~lc3.reg[lc3.source_reg_1];
            update_flag(lc3);
            break;
        case LDI:
        /*
            LDI DR, LABEL
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.pc_offset = SIGN_EXTEND((lc3.instr & 0x1ff),9);
            lc3.reg[lc3.dest_reg] = MEM_READ(MEM_READ(lc3.reg[PC]+lc3.pc_offset));
            update_flag(lc3);
            break;
        case STI:
        /*
            STI  SR, LABEL
        */
            lc3.source_reg_1 = (lc3.instr >> 9) & 0x7;
            lc3.pc_offset = SIGN_EXTEND(lc3.instr & 0x1ff,9);
            MEM_WRITE(MEM_READ(reg[PC]+lc3.pc_offset),lc3.reg[lc3.source_reg_1]);
            break;
        case JMP:
        /*
            JMP BaseR
            RET 
            return loads return address form R7 and jumps to it. 
            The RET instruction is defined with dest reg as R7
        */
            lc3.dest_reg = (lc3.instr >> 6) & 0x7;
            lc3.reg[PC] = lc3.reg[lc3.dest_reg];
            break;
        case RES:
            perror("BAD INSTRUCTION");
            exit(1);
            break;
        case LEA:
        /*
            LEA  DR, LABEL
        */
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.pc_offset = SIGN_EXTEND(lc3.instr & 0x1ff,9);
            lc3.reg[lc3.dest_reg] = lc3.reg[PC] + lc3.pc_offset;    
            break;
        case TRAP:
        /*
            Trap Routines can be considered as the operating system API, 
            these are some predefined subroutines that perform common tasks and interact with I/O.
            Instead of writing our own primitive subroutine we will use the original windows os calls.
        */
            lc3.reg[R7] = lc3.reg[PC];
            switch (lc3.instr & 0xff){
                case TRAP_GETC:
                /*
                    Reads ascii characters
                */
                    lc3.reg[R0] = (uint16_t)getchar();
                    lc3.dest_reg = R0;
                    update_flag(lc3);
                    break;
                case TRAP_PUTS:
                /*
                    PUTS writes a string of ascii characters to the console display. 
                    Starts with the memory address stored in R0, writing is null terminated. 
                */
                    uint16_t *c = memory + lc3.reg[R0];
                    while(*c){
                        putc((char)*c,stdout); // prints it on the terminal. 
                        ++c;
                    }
                    fflush(stdout); // flushes the buffer
                    break;
                case TRAP_IN:
                /*
                    Reads and prints the ascii character read on the terminal.
                */
                    printf("Enter a character:");
                    char input_char = getchar();
                    putc(input_char, stdout);
                    lc3.reg[R0] = (uint16_t)input_char;
                    fflush(stdout);
                    lc3.dest_reg = R0;
                    update_flag(lc3);
                    break;
                case TRAP_OUT:
                /*
                    Kinda like the print statement, outputs stuff from R0 onto terminal
                */
                    putc((char)lc3.reg[R0],stdout);
                    fflush(stdout);
                    break;
                case TRAP_PUTSP:
                /*
                    Outputs string
                */
                    uint16_t*c = memory+lc3.reg[R0];
                    while(*c){
                        char char1 = (*c)&0xff;
                        putc(char1,stdout);
                        char char2 = (*c) >> 8;
                        if(char2) putc(char2, stdout);
                        putc(char2,stdout);
                        ++c;
                    }
                    break;
                case TRAP_HALT:
                    puts("HALT");
                    fflush(stdout);
                    lc3.running = 0;
                    break;
                default:
                    perror("BAD INSTRUCTION");
                    exit(1);
                    break;
            }
            break;
        default:
        perror("BAD INSTRUCTION");
        exit(1);
            break;
        }
    }
}