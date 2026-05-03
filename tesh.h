#ifndef TESt_H
#define TEST_H

#include "cpu.h"

#include <stdio.h>

// LDI and ADD
static void TEST_HardcodeTest1(Cpu *cpu) {
    // LDI 0 0x02
    cpu->RAM[0] = 0x10;
    cpu->RAM[1] = 0x02;
    // LDI 0 0x3
    cpu->RAM[2] = 0x11;
    cpu->RAM[3] = 0x03;
    // ADD 0 1
    cpu->RAM[4] = 0x50;
    cpu->RAM[5] = 0x10;
    // HLT
    cpu->RAM[6] = 0x0F;
    cpu->RAM[7] = 0x00;
}

//memory and reg moving
static void TEST_HardcodeTest2(Cpu *cpu) {
    // LDI 0 0x02
    cpu->RAM[0] = 0x10;
    cpu->RAM[1] = 0x02;
    // STM 0 10
    cpu->RAM[2] = 0x30;
    cpu->RAM[3] = 0x12;
    // LDM 2 10
    cpu->RAM[4] = 0x22;
    cpu->RAM[5] = 0x12;
    // HLT
    cpu->RAM[6] = 0x0F;
    cpu->RAM[7] = 0x00;
}

// Print cpu state
static void TEST_PrintState(Cpu *cpu, int fl_reg) {
    if (fl_reg) {
        printf("REG : %d %d %d %d\n", cpu->R[0], cpu->R[1], cpu->R[2], cpu->R[3]);
    }
}

// Print ram state
static void TEST_PrintRam(Cpu *cpu) {
    printf("RAM State:\n");
    for (int i = 0; i < 256; i++) {
        if (i % 16 == 0) printf("\n");
        printf("0x%02X ", cpu->RAM[i]);
    }
}

#endif