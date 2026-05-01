#ifndef ROM_H
#define ROM_H

#include <stdio.h>
#include "cpu.h"

// Load the rom into the ram
int ROM_LoadProgram(Cpu *cpu, const char *RomName) {
    FILE *file = fopen(RomName, "rb");
    if (!file) {
        printf("Failed to open %s\n", RomName);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long romLength = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (romLength > 240) {
        printf("Program is too long (max size = 240 Bytes)\n");
        return 2;
    }

    fread(&cpu->RAM[0], romLength, 1, file);
    fclose(file);
    return 0;
}


#endif