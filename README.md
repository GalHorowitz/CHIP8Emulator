# CHIP-8 Emulator
This is a [CHIP-8](https://wikipedia.org/wiki/CHIP-8) emulator written in C++ for Windows.

## How to use
I tested my emulator using ROMs from [badlogic's repo](https://github.com/badlogic/chip8/tree/master/roms).
```batch
CHIP8Emulator.exe roms\pong2.rom
```

## TODO
- Sound output currently does not work (The sound register does decrement every 1/60 of a second, but no tone is heard)
    It seems that on Windows if I want to play sound with full control of timing I need to use quite a bit code if I don't
    want to use a library. I need to find the simplest path to interface with Windows.
- I should probably add an option for the user to set the simulated clock HZ, as different games assume different clock speeds.

## References
The code is mainly based on [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).
The Windows bindings are based on the first few episodes of [Casey Muratori's Handmade Hero](https://guide.handmadehero.org) series.