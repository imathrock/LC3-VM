#include "LC3.h"

static LC3 cpu;

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

int read_image(const char* image_path){
    FILE* file = fopen(image_path, "rb");
    if (!file) { return 0; };
    read_image_file(file);
    fclose(file);
    return 1;
}

static const char *cond_str(uint16_t cond) {
    switch (cond) {
        case 1: return "P";
        case 2: return "Z";
        case 4: return "N";
        default: return "—";
    }
}

void print_state(const LC3 *cpu) {
    printf("==== LC3 STATE ====\n");
    printf("PC: 0x%04X  COND: %s  RUN: %d\n",
           cpu->PC, cond_str(cpu->cond), cpu->running);
    for (int i = 0; i < COUNT; i++)
        printf("R%d: 0x%04X  ", i, cpu->reg[i]);
    printf("\n\n");
    printf("INSTR: 0x%04X  OP: %u\n", cpu->instr, cpu->op);
    printf("DR: %u  SR1: %u  SR2: %u  IMM_VAL: 0x%04X  PC_OFF: %d\n",
           cpu->dr, cpu->sr1, cpu->sr2,
            cpu->imm_val, (int16_t)cpu->pc_offset);
    printf("===================\n");
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
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();
    cpu.PC = 0x3000;
    cpu.cond = ZRO;
    cpu.running = 1;
    while(cpu.running){
        cpu.instr = MEM_READ(cpu.PC++);
        cpu.op = get_op(cpu.instr);
        switch (cpu.op){
        case BR:
            /* BRnzp LABEL */
           cpu.pc_offset = pc_offset(cpu.instr);
           if(cond(cpu.instr) & cpu.cond){
                cpu.PC += cpu.pc_offset;
           }
            break;
        case ADD:
            /* ADD DR, SR1, SR2 */
            cpu.dr = dr(cpu.instr);
            cpu.sr1 = sr1(cpu.instr);
            if((cpu.instr >> 5) & 0x1){
                cpu.imm_val = imm5(cpu.instr);
                cpu.reg[cpu.dr] = cpu.reg[cpu.sr1] + cpu.imm_val;
            }else{
                cpu.sr2 = sr2(cpu.instr);
                cpu.reg[cpu.dr] = cpu.reg[cpu.sr1] + cpu.reg[cpu.sr2];
            }
            setcc(&cpu, cpu.dr);
            break;
        case LD:
            /* LD DR, PC_OFFSET */
            cpu.dr = dr(cpu.instr);
            cpu.pc_offset = pc_offset(cpu.instr);
            cpu.reg[cpu.dr] = MEM_READ(cpu.PC + cpu.pc_offset);
            setcc(&cpu, cpu.dr);
            break;
        case ST:
            /* ST SR, LABEL */
            cpu.sr1 = dr(cpu.instr);  // ST uses bits 9-11 for source register
            cpu.pc_offset = pc_offset(cpu.instr);
            MEM_WRITE(cpu.PC + cpu.pc_offset , cpu.reg[cpu.sr1]);
            break;
        case JSR:
            /*  JSR  Label
                JSRR  BaseR */
            cpu.reg[R7] = cpu.PC;
            if(cpu.instr&0x0800){
                cpu.pc_offset = pcoffset11(cpu.instr);
                cpu.PC += cpu.pc_offset;
            }else{
                cpu.sr1 = baseR(cpu.instr);
                cpu.PC = cpu.reg[cpu.sr1];
            }
            break;
        case AND:
            /* AND DR, SR1, SR2 */
            cpu.dr = dr(cpu.instr);
            cpu.sr1 = sr1(cpu.instr);
            if((cpu.instr >> 5) & 0x1){
                cpu.imm_val = imm5(cpu.instr);
                cpu.reg[cpu.dr] = cpu.reg[cpu.sr1] & cpu.imm_val;
            }else{
                cpu.sr2 = sr2(cpu.instr);
                cpu.reg[cpu.dr] = cpu.reg[cpu.sr1] & cpu.reg[cpu.sr2];
            }
            setcc(&cpu, cpu.dr);
            break;
        case LDR:
            /* LDR DR, baseR, offset6 */
            cpu.dr = dr(cpu.instr);
            cpu.sr1 = baseR(cpu.instr);
            cpu.imm_val = offset6(cpu.instr);
            cpu.reg[cpu.dr] = MEM_READ(cpu.reg[cpu.sr1] + cpu.imm_val);
            setcc(&cpu, cpu.dr);
            break;
        case STR:
            /* STR SR, baseR, offset6 */
            cpu.dr = dr(cpu.instr);
            cpu.sr1 = baseR(cpu.instr);
            cpu.imm_val = offset6(cpu.instr);
            MEM_WRITE(cpu.reg[cpu.sr1] + cpu.imm_val, cpu.reg[cpu.dr]);
            break;
        case RTI:
            /* Unused */
            perror("BAD INSTRUCTION: RTI");
            exit(1);
            break;
        case NOT:
            /* NOT DR, SR */
            cpu.dr = dr(cpu.instr);
            cpu.sr1 = sr1(cpu.instr);
            cpu.reg[cpu.dr] = ~cpu.reg[cpu.sr1];
            setcc(&cpu, cpu.dr);
            break;
        case LDI:
            /* LDI DR, LABEL */
            cpu.dr = dr(cpu.instr);
            cpu.pc_offset = pc_offset(cpu.instr);
            cpu.reg[cpu.dr] = MEM_READ(MEM_READ(cpu.PC + cpu.pc_offset));
            setcc(&cpu, cpu.dr);
            break;
        case STI:
            /* STI DR, LABEL */
            cpu.dr = dr(cpu.instr);
            cpu.pc_offset = pc_offset(cpu.instr);
            MEM_WRITE(MEM_READ(cpu.PC + cpu.pc_offset), cpu.reg[cpu.dr]);
            break;
        case JMP:
            /* JMP baseR
               RET       */
            cpu.sr1 = baseR(cpu.instr);
            cpu.PC = cpu.reg[cpu.sr1];
            break;
        case RES:
            /* Reserved */
            perror("BAD INSTRUCTION: RES");
            exit(0);
            break;
        case LEA:
            /* LEA DR, LABEL */
            cpu.dr = dr(cpu.instr);
            cpu.pc_offset = pc_offset(cpu.instr);
            cpu.reg[cpu.dr] = cpu.PC + cpu.pc_offset;
            setcc(&cpu, cpu.dr);
            break;
        case TRAP:
            /* TRAP trapvector8 */
            cpu.reg[R7] = cpu.PC;
            switch (trapvec(cpu.instr)){
                case TRAP_GETC:
                    /* gets character from keyboard */
                    cpu.reg[R0] = (uint16_t)getchar();
                    setcc(&cpu, R0);
                    break;
                case TRAP_OUT:
                    /* Outputs a character */
                    putc((char)cpu.reg[R0], stdout);
                    fflush(stdout);
                    break;
                case TRAP_PUTS:
                    /* Prints a word string */
                    uint16_t* c = memory + cpu.reg[R0];
                    while (*c){
                        putc((char)*c, stdout);
                        ++c;
                    }
                    fflush(stdout);
                    break;
                case TRAP_IN:
                    /* Takes input form keyboard and prints it on terminal */
                    printf("Enter a character: ");
                    char cc = getchar();
                    putc(cc, stdout);
                    fflush(stdout);
                    cpu.reg[R0] = (uint16_t)cc;
                    setcc(&cpu, R0);
                    break;
                case TRAP_PUTSP:
                    /* Prints out the whole string */
                    uint16_t* csp = memory + cpu.reg[R0];
                    while (*csp)
                    {
                        char char1 = (*csp) & 0xFF;
                        putc(char1, stdout);
                        char char2 = (*csp) >> 8;
                        if (char2) putc(char2, stdout);
                        ++csp;
                    }
                    fflush(stdout);
                    break;
                case TRAP_HALT:
                    /* HALTS the CPU */
                    printf("\n HALT \n");
                    fflush(stdout);
                    cpu.running = 0;
                    break;

                default:
                    
                    printf("Unknown trap vector: 0x%02x\n", trapvec(cpu.instr));
                    perror("BAD INSTRUCTION: TRAP ROUTINE FUCKED");
                    exit(0);
                    break;
                }
            break;

        default :
        
        perror("BAD INSTRUCTION: SOMETHING IS FUCKED UP");
        exit(0);
            break;
        }
    }
    restore_input_buffering();
    return 0;
}