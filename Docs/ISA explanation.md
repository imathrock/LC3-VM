# ISA explained
This document intends to explain what each of the LC3 instruction does. I will put up proper explainations later on. 

## List of all Little Computer 3 instructions
This document does a better job than I ever can: [LC3-ISA](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf)

## Future Plans: List of all RV32I instructions
This has been taken from ChatGPT.
### 1. Integer Computational Instructions (RV32I)

### Register–Register Arithmetic

```
add   rd, rs1, rs2   # rd = rs1 + rs2
sub   rd, rs1, rs2   # rd = rs1 - rs2
sll   rd, rs1, rs2   # rd = rs1 %3C< (rs2 & 0x1F)
srl   rd, rs1, rs2   # rd = rs1 %3E> (rs2 & 0x1F) logical
sra   rd, rs1, rs2   # rd = rs1 >> (rs2 & 0x1F) arithmetic
slt   rd, rs1, rs2   # rd = (rs1 < rs2) ? 1 : 0 (signed)
sltu  rd, rs1, rs2   # same but unsigned
xor   rd, rs1, rs2   # rd = rs1 ^ rs2
or    rd, rs1, rs2   # rd = rs1 | rs2
and   rd, rs1, rs2   # rd = rs1 & rs2
```

### Immediate Arithmetic

```
addi  rd, rs1, imm   # rd = rs1 + imm
slti  rd, rs1, imm   # rd = (rs1 < imm) ? 1 : 0 (signed)
sltiu rd, rs1, imm   # unsigned version
xori  rd, rs1, imm   # rd = rs1 ^ imm
ori   rd, rs1, imm   # rd = rs1 | imm
andi  rd, rs1, imm   # rd = rs1 & imm
slli  rd, rs1, shamt # rd = rs1 << shamt
srli  rd, rs1, shamt # logical right shift
srai  rd, rs1, shamt # arithmetic right shift
```



## 2. Control Transfer

### Unconditional Jumps

```
jal   rd, offset     # rd = PC+4; PC += offset
jalr  rd, rs1, imm   # rd = PC+4; PC = (rs1+imm)&~1
```

### Conditional Branches

```
beq   rs1, rs2, offset  # if rs1==rs2 jump
bne   rs1, rs2, offset  # if rs1!=rs2 jump
blt   rs1, rs2, offset  # if rs1<rs2 signed
bge   rs1, rs2, offset  # if rs1>=rs2 signed
bltu  rs1, rs2, offset  # unsigned
bgeu  rs1, rs2, offset  # unsigned
```



## 3. Memory Access

### Loads

```
lb   rd, offset(rs1)   # load byte sign-extended
lh   rd, offset(rs1)   # load halfword sign-extended
lw   rd, offset(rs1)   # load word
lbu  rd, offset(rs1)   # load byte zero-extended
lhu  rd, offset(rs1)   # load halfword zero-extended
```

### Stores

```
sb   rs2, offset(rs1)  # store byte
sh   rs2, offset(rs1)  # store halfword
sw   rs2, offset(rs1)  # store word
```



## 4. Upper Immediate

```
lui   rd, imm20    # rd = imm20 << 12
auipc rd, imm20    # rd = PC + (imm20 << 12)
```



## 5. System Instructions

```
ecall    # environment call (syscall)
ebreak   # debugger breakpoint
csrrw rd, csr, rs1   # atomic read/write CSR
csrrs rd, csr, rs1   # read + set bits
csrrc rd, csr, rs1   # read + clear bits
csrrwi rd, csr, imm  # same with immediate
csrrsi rd, csr, imm
csrrci rd, csr, imm
```



## 6. Multiplication and Division (M Extension)

```
mul    rd, rs1, rs2   # lower 32 bits
mulh   rd, rs1, rs2   # upper 32 bits signed* signed
mulhsu rd, rs1, rs2   # signed * unsigned
mulhu  rd, rs1, rs2   # unsigned * unsigned
div    rd, rs1, rs2   # signed division
divu   rd, rs1, rs2   # unsigned division
rem    rd, rs1, rs2   # signed remainder
remu   rd, rs1, rs2   # unsigned remainder
```



## 7. Atomic Instructions (A Extension, RV32A)

```
lr.w   rd, (rs1)       # load-reserved
sc.w   rd, rs2, (rs1)  # store-conditional
amoswap.w rd, rs2, (rs1) # swap
amoadd.w  rd, rs2, (rs1) # atomic add
amoand.w  rd, rs2, (rs1) # atomic and
amoor.w   rd, rs2, (rs1) # atomic or
amoxor.w  rd, rs2, (rs1) # atomic xor
amomin.w  rd, rs2, (rs1) # atomic min signed
amomax.w  rd, rs2, (rs1) # atomic max signed
amominu.w rd, rs2, (rs1) # atomic min unsigned
amomaxu.w rd, rs2, (rs1) # atomic max unsigned
```



## 8. Floating-Point Instructions (F/D Extensions)

Examples for RV32F (single precision):

```
flw   ft0, offset(rs1)   # load float
fsw   ft0, offset(rs1)   # store float
fadd.s fd, fs1, fs2      # single precision add
fsub.s fd, fs1, fs2      # subtract
fmul.s fd, fs1, fs2      # multiply
fdiv.s fd, fs1, fs2      # divide
fsqrt.s fd, fs1          # sqrt
fsgnj.s fd, fs1, fs2     # sign-inject
fmin.s fd, fs1, fs2      # min
fmax.s fd, fs1, fs2      # max
fcvt.w.s rd, fs1         # float to int
fcvt.s.w fd, rs1         # int to float
feq.s rd, fs1, fs2       # compare equal
flt.s rd, fs1, fs2       # compare less
fle.s rd, fs1, fs2       # compare less/equal
```

Double precision (RV32D/RV64D) has `.d` versions.


## 9. Compressed Instructions (C Extension)

Compressed 16-bit encodings, e.g.:

```
c.addi rd, imm
c.li   rd, imm
c.lw   rd, offset(rs1)
c.sw   rs2, offset(rs1)
c.j    offset
c.beqz rs1, offset
c.bnez rs1, offset
```

## 10. Vector Instructions (V Extension)

Example subset:

```
vadd.vv vd, vs1, vs2   # vector add
vsub.vv vd, vs1, vs2   # vector subtract
vmul.vv vd, vs1, vs2   # vector multiply
vdiv.vv vd, vs1, vs2   # vector divide
vle32.v vd, (rs1)      # load vector
vse32.v vd, (rs1)      # store vector
```
