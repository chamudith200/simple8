#include <stdio.h>

#include "cpu.h"
#include "rom.h"

int main(int argc, char **argv) {
    Cpu cpu;

    CPU_Init(&cpu);
    if (ROM_LoadProgram(&cpu, "rom1.rom") != 0) {
        return 1;
    } 
    CPU_Run(&cpu);

    return 0;
}


