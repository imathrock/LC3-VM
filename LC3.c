#include "LC3.h"

static LC3 lc3;

int main(int argc, const char*argv){
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
        lc3.instr = mem_read(reg[PC]++);
        lc3.op = lc3.instr >> 12;
        switch (lc3.op)
        {
        case BR:
        /*
            Conditional branch instruction. conditions matched with flags provided
            PC <- PC+ PC_OFFSET
        */
            lc3.pc_offset = SIGN_EXTEND(lc3.instr & 0x1ff,9);
            if(lc3.reg[COND] & (uint16_t)((lc3.instr >> 9) &0x7)){ // Did not want to allocate on stack so 
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
                    
                    break;
                case TRAP_PUTS:
                    break;
                case TRAP_IN:
                    break;
                case TRAP_OUT:
                    break;
                case TRAP_PUTSP:
                    break;
                case TRAP_HALT:
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