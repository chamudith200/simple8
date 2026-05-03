# Simple8

A minimal byte-wide CPU emulator with a custom designed architecture.
Feed it a binary ROM, it executes it, outputs to terminal. That's it.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the full ISA spec.

---

## Build

Requires GCC. On Windows use MinGW or MSYS2.

```bash
gcc -o simple8 main.c cpu.c
```

---

## Run

```bash
./simple8.exe program.rom
```

The ROM is a flat binary file loaded into memory starting at `0x00`.
Maximum program size is 256 bytes (128 instructions). The stack grows down from `0x00`. The program snd the stack can overwrite others. 

---

## Notes

- All 256 bytes of RAM is a flat address space — no memory protection
- Stack starts at `0xFF` and grows downward into program space if unchecked
- The machine will execute whatever bytes PC points at, no exceptions