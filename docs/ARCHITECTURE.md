# SIMPLE 8
## Custom CPU Architecture Spec — v1

A minimal byte-wide CPU architecture designed for simplicity.  
Input is a flat binary ROM. Output is terminal via dedicated I/O instructions.  
No OS. No syscalls. No memory management. Just a state machine.  
No handbrakes. Undefined behavior is the programmer's problem.

---

## CPU Structure

| Field | Type | Description | Initial Value |
|-------|------|-------------|---------------|
| `R[4]` | `uint8_t` | General purpose registers R0–R3 | `0x00` |
| `PC` | `uint8_t` | Program counter | `0x00` |
| `SP` | `uint8_t` | Stack pointer, grows downward | `0xFF` |
| `FL` | `uint8_t` | Flags register | `0x00` |
| `RAM[256]` | `uint8_t` | Flat memory space | — |

---

## Flags Register (FL)

FL is a `uint8` where each bit is an independent boolean flag. Default state is `0x00`.

| Bit | Flag | Name | Set when |
|-----|------|------|----------|
| 0 | Z | Zero | Result is zero |
| 1 | C | Carry | Arithmetic carry or overflow |
| 2 | N | Negative | Result bit 7 is set |
| 3–7 | — | Unused | Reserved |

**Flag masks:**
```
FL_ZERO     = 0x01
FL_CARRY    = 0x02
FL_NEGATIVE = 0x04
```

Flags are **only** updated by ALU instructions. All other instructions leave FL untouched.

| Instruction | Flags affected |
|-------------|---------------|
| ADD, SUB | Z, C, N |
| AND, OR, XOR | Z |
| NOT | Z |
| INC | Z, C |
| DEC | Z, N |

---

## Memory Map

The entire 256 bytes is one flat address space. Program loads at `0x00`. Stack starts at `0xFF` and grows downward. There is no boundary enforcement. The stack can overwrite the program and the program can overwrite the stack. This is by design.

---

## Instruction Format

Every instruction is exactly **2 bytes / 4 nibbles**:

```
byte 0            byte 1
[ OP ][ N2 ]      [ N3 ][ N4 ]
 4bit   4bit       4bit   4bit
```

| Field | Description |
|-------|-------------|
| `OP` | Opcode — first nibble, `0x0`–`0xF` |
| `N2` | Sub-opcode or operand |
| `N3` | Operand |
| `N4` | Operand |
| `B2` | Full second byte (`N3:N4`), used as immediate value or address |

Unused nibbles are ignored. `0x0` is preferred by convention.  
Programmer is responsible for initializing all registers before use.

---

## Instruction Set

### Data Movement

| Opcode | Mnemonic | Encoding | Description |
|--------|----------|----------|-------------|
| `0x0` | NOP | `[0x0][0x0][0][0]` | No operation |
| `0x0` | HLT | `[0x0][0xF][0][0]` | Halt execution |
| `0x1` | LDI Rd, imm | `[0x1][Rd][B2]` | Load immediate into Rd |
| `0x2` | LDM Rd, addr | `[0x2][Rd][B2]` | Load RAM[addr] into Rd |
| `0x3` | STM Rs, addr | `[0x3][Rs][B2]` | Store Rs into RAM[addr] |
| `0x4` | MOV Rd, Rs | `[0x4][Rd][Rs][0]` | Copy Rs into Rd |

### ALU

| Opcode | Mnemonic | Encoding | Description |
|--------|----------|----------|-------------|
| `0x5` | ADD Rd, Rs | `[0x5][Rd][Rs][0]` | Rd = Rd + Rs |
| `0x6` | SUB Rd, Rs | `[0x6][Rd][Rs][0]` | Rd = Rd − Rs |

### Bitwise Group (`0x7`)

| N2 | Mnemonic | Encoding | Description |
|----|----------|----------|-------------|
| `0x0` | AND Rd, Rs | `[0x7][0x0][Rd][Rs]` | Rd = Rd & Rs |
| `0x1` | OR Rd, Rs | `[0x7][0x1][Rd][Rs]` | Rd = Rd \| Rs |
| `0x2` | XOR Rd, Rs | `[0x7][0x2][Rd][Rs]` | Rd = Rd ^ Rs |

### Unary Group (`0x8`)

| N2 | Mnemonic | Encoding | Description |
|----|----------|----------|-------------|
| `0x0` | NOT Rd | `[0x8][0x0][Rd][0]` | Rd = ~Rd |
| `0x1` | INC Rd | `[0x8][0x1][Rd][0]` | Rd = Rd + 1 |
| `0x2` | DEC Rd | `[0x8][0x2][Rd][0]` | Rd = Rd − 1 |

### Jump Group (`0x9`)

| N2 | Mnemonic | Encoding | Description |
|----|----------|----------|-------------|
| `0x0` | JMP addr | `[0x9][0x0][B2]` | Unconditional jump |
| `0x1` | JZ addr | `[0x9][0x1][B2]` | Jump if Z set |
| `0x2` | JNZ addr | `[0x9][0x2][B2]` | Jump if Z not set |
| `0x3` | JN addr | `[0x9][0x3][B2]` | Jump if N set |
| `0x4` | JC addr | `[0x9][0x4][B2]` | Jump if C set |

### Subroutines

| Opcode | Mnemonic | Encoding | Description |
|--------|----------|----------|-------------|
| `0xA` | CALL addr | `[0xA][0][B2]` | Push PC, jump to addr |
| `0xB` | RET | `[0xB][0][0][0]` | Pop PC, return |

### Stack

| Opcode | Mnemonic | Encoding | Description |
|--------|----------|----------|-------------|
| `0xC` | PUSH Rs | `[0xC][Rs][0][0]` | Push Rs onto stack |
| `0xD` | POP Rd | `[0xD][Rd][0][0]` | Pop from stack into Rd |

### I/O Group (`0xE`)

| N2 | Mnemonic | Encoding | Description |
|----|----------|----------|-------------|
| `0x0` | OUT Rd | `[0xE][0x0][Rd][0]` | Write Rd to terminal |
| `0x1` | IN Rd | `[0xE][0x1][Rd][0]` | Read from terminal into Rd |

---

## Fetch Cycle

```
opcode = RAM[PC],  PC++
arg    = RAM[PC],  PC++
```

PC is a `uint8`. It does not wrap. If it runs past the program it will execute
whatever is in memory at that point. The machine is too dumb to care.