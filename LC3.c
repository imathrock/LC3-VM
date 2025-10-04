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
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            break;
        case ADD:
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
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.pc_offset = SIGN_EXTEND((lc3.instr & 0x1ff),9);
            lc3.reg[lc3.dest_reg] = MEM_READ(lc3.reg[PC]+lc3.pc_offset);
            break;
        case ST:
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            break;
        case JSR:
            /* code */
            break;
        case AND:
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
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            break;
        case STR:
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
            lc3.ival = SIGN_EXTEND((lc3.instr&0x1f), 5);
            MEM_WRITE(lc3.reg[lc3.source_reg_1]+lc3.ival,lc3.reg[lc3.dest_reg]);
            break;
        case NOT:
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.source_reg_1 = (lc3.instr >> 6) & 0x7;
            lc3.reg[lc3.dest_reg] = ~lc3.reg[lc3.source_reg_1];
            break;
        case LDI:
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            lc3.pc_offset = SIGN_EXTEND((lc3.instr & 0x1ff),9);
            lc3.reg[lc3.dest_reg] = mem_read(mem_read(lc3.reg[PC]+lc3.pc_offset));
            update_flag(lc3);
            break;
        case STI:
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            break;
        case JMP:
            /* code */
            break;
        case RES:
            /* code */
            break;
        case LEA:
            lc3.dest_reg = (lc3.instr >> 9) & 0x7;
            break;
        case TRAP:
            /* code */
            break;
        case HALT:
        lc3.running = 0;
            break;
        default:
        perror("BAD INSTRUCTION");
        exit(1);
            break;
        }
    }
}