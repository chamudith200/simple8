#ifndef CPU
#define CPU

#include <stdint.h>

// CHIP structure
typedef struct{
    uint8_t R[4];       // registers r0...r3
    uint8_t PC;         // program counter
    uint8_t SP;         // stack pointer
    uint8_t FL;         // flags
    uint8_t RAM[256];   // RAM
}Cpu;

// CPU flags
enum{
    FL_ZERO = 0x1,
    FL_CARRY = 0x2,
    FL_NEGATIVE = 0x4
}CPU_Flags;

enum{
    STACK_START = 0xFF,
    STACK_END = 0xF0
};

// Initialize the CPU
void CPU_Init(Cpu *cpu);

// CPU mainloop
int CPU_Run(Cpu *cpu);

// Fetch an instruction from thr ram into the CPU
uint8_t *CPU_Fetch(Cpu *cpu);



#endif