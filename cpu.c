#include "cpu.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// Intialize the cpu
void CPU_Init(Cpu *cpu) {
    cpu->PC = 0x00;
    cpu->SP = 0xFF;
    memset(cpu->RAM, 0, sizeof(cpu->RAM));
    memset(cpu->R, 0, sizeof(cpu->R));
    cpu->FL = 0b00000000;
}

// CPU mainloop
int CPU_Run(Cpu *cpu) {
    int running = 1;

    while (running) {
        // Fetch execute cycle
        uint8_t *instructions = CPU_Fetch(cpu);

        // Sleep for half a sec
        usleep(500000);
    }

    return 0;

}

// Fetch an instruction from thr ram into the CPU
uint8_t *CPU_Fetch(Cpu *cpu) {
    uint8_t instruction[2];

    instruction[0] = cpu->RAM[cpu->PC];
    cpu->PC++;
    instruction[1] = cpu->RAM[cpu->PC];
    cpu->PC++;

    return instruction;
}