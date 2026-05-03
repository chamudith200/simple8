#include "cpu.h"
#include "tesh.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define TENTH_A_SEC 100000

// Intialize the cpu
void CPU_Init(Cpu *cpu) {
    cpu->PC = 0x00;
    cpu->SP = (uint8_t)STACK_START;
    memset(cpu->RAM, 0, sizeof(cpu->RAM));
    memset(cpu->R, 0, sizeof(cpu->R));
    cpu->FL = 0b00000000;
    
}

// CPU mainloop
int CPU_Run(Cpu *cpu) {
    int running = 1;
    uint8_t instructions[2];
    while (running) {
        // Fetch execute cycle
        uint8_t instructions[2];
        CPU_Fetch(cpu, instructions);
        CPU_Exec(cpu, instructions, &running);
        if (!running) break;
        TEST_PrintState(cpu, 1, 1);

        // Sleep for half a sec
        usleep(TENTH_A_SEC * 1);
    }
    TEST_PrintRam(cpu, 0);

    return 0;

}

// Fetch an instruction from the ram into the CPU
uint8_t *CPU_Fetch(Cpu *cpu, uint8_t instructions[2]) {

    instructions[0] = cpu->RAM[cpu->PC];
    cpu->PC++;
    instructions[1] = cpu->RAM[cpu->PC];
    cpu->PC++;
}

void CPU_Exec(Cpu *cpu, uint8_t instructions[2], int *running) {
    uint8_t OP = (instructions[0] & 0xF0) >> 4;
    uint8_t N2 = instructions[0] & 0b00001111;
    uint8_t N3 = (instructions[1] & 0xF0) >> 4;
    uint8_t N4 = instructions[1] & 0b00001111;
    uint8_t B2 = instructions[1];

    switch (OP) {
        case 0x0:   // NOP/HLT
            if (N2 == 0xF) {
                *running = 0;
                printf("HLT\n");
            }
            break;

        case 0x1:   // LDI
            cpu->R[N2] = B2;
            break;

        case 0x2:   // LDM
            cpu->R[N2] = cpu->RAM[B2];
            break;

        case 0x3:   // STM
            cpu->RAM[B2] = cpu->R[N2];
            break;

        case 0x4:   // MOV
            cpu->R[N2] = cpu->R[N3];
            break;

        case 0x5: {  // ADD
            cpu->FL &= ~(FL_CARRY | FL_ZERO);
            int add = cpu->R[N2] + cpu->R[N3];
            if (add == 0x0) cpu->FL |= FL_ZERO;
            if (add > 0xFF) cpu->FL |= FL_CARRY;
            cpu->R[N2] = add;
            break;
        }

        case 0x6:{  // SUB
            cpu->FL &= ~(FL_NEGATIVE | FL_ZERO);
            int sub = cpu->R[N2] - cpu->R[N3];
            if (sub == 0x0) cpu->FL |= FL_ZERO;
            if (sub < 0X0) cpu->FL |= FL_NEGATIVE;
            cpu->R[N2] = sub;
            break;
        }

        case 0x7:   // bitwise OPR
            switch (N2) {
                case 0x0:{  // AND
                    cpu->FL &= ~(FL_ZERO);
                    int and = cpu->R[N3] & cpu->R[N4];
                    if (and == 0x0) cpu->FL |= FL_ZERO;
                    cpu->R[N3] = and;
                    break;
                }  
                case 0x1:{  // OR
                    cpu->FL &= ~(FL_ZERO);
                    int or = cpu->R[N3] | cpu->R[N4];
                    if (or == 0x0) cpu->FL |= FL_ZERO;
                    cpu->R[N3] = or;
                    break;
                }
                case 0x2:{  // XOR
                    cpu->FL &= ~(FL_ZERO);
                    int xor = cpu->R[N3] ^ cpu->R[N4];
                    if (xor == 0x0) cpu->FL |= FL_ZERO;
                    cpu->R[N3] = xor;
                    break;
                }    
            }

        case 0x8:   // unary
            switch (N2) {
                case 0x0:   // NOT
                    cpu->FL &= ~(FL_ZERO);
                    cpu->R[N3] = ~cpu->R[N3];
                    if (cpu->R[N3] == 0x0) cpu->FL |= FL_ZERO;
                    break;

                case 0x1:   // INC
                    cpu->FL &= ~(FL_CARRY);
                    if (cpu->R[N3] == 0xFF) {
                        cpu->FL |= FL_CARRY;
                        cpu->R[N3] = 0x00;
                        break;
                    }
                    cpu->R[N3]++;   
                    break;

                case 0x2:   // DEC
                    cpu->FL &= ~(FL_NEGATIVE | FL_ZERO);
                    if(cpu->R[N3] == 0x00) {
                        printf("DEC UNDERFLOW\n");
                        cpu->FL |= FL_NEGATIVE;
                        cpu->R[N3] = 0xFF;
                        break;
                    }
                    cpu->R[N3]--;
                    if (cpu->R[N3] == 0x0) cpu->FL |= FL_ZERO;
                    break;
            }
            break;

        case  0x9:  // JUMP
            switch (N2) {
                case 0x0:   // JMP
                    cpu->PC = cpu->R[N3];
                    break;

                case 0x1:   // JZ
                    if (cpu->FL & FL_ZERO == FL_ZERO) cpu->PC = cpu->R[N3];
                    break;

                case 0x2:   // JNZ
                    if (cpu->FL & FL_ZERO != FL_ZERO) cpu->PC = cpu->R[N3];
                    break;

                case 0x3:   // JN
                    if (cpu->FL & FL_NEGATIVE == FL_NEGATIVE) cpu->PC = cpu->R[N3];
                    break;

                case 0x4:   // JC
                    if (cpu->FL & FL_CARRY == FL_CARRY) cpu->PC = cpu->R[N3];
                    break;
            }
            break;

        case 0xA:   // CALL
            cpu->RAM[cpu->SP] = cpu->PC;
            cpu->SP--;
            cpu->PC = B2;
            break;

        case 0xB:   // RET
            cpu->SP++;
            cpu->PC = cpu->RAM[cpu->SP];
            break;

        case 0xC:   // PUSH
            cpu->RAM[cpu->SP] = cpu->R[N2];
            cpu->SP--;
            break;
        
        case 0xD:   // POP
            cpu->SP++;
            cpu->R[N2] = cpu->RAM[cpu->SP];
            break;
            
        case 0xE:   // I/O
            switch (N2) {
                case 0x0:   // write to terminal
                    putchar(cpu->R[N3]);
                    printf(" : 0x%02X\n", cpu->R[N3]);
                    break;

                case 0x1:   // Read from the terminal
                    cpu->R[N3] = (uint8_t)getchar();
                    break;
            }
            break;

        case 0XF:
            break;
    }
}

