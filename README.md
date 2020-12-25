# CHIP-8 Emulator
This is a [CHIP-8](https://wikipedia.org/wiki/CHIP-8) emulator written in C++ for Windows, using no libraries.
The 64x32 pixels screen is mapped to a 640x320 pixel window.
The original hex keyboard is mapped to the following keys on a standard qwerty keyboard:

#### CHIP-8 Hex Keyboard
<table><tbody><tr><td>1</td><td>2</td><td>3</td><td>C</td></tr><tr><td>4</td><td>5</td><td>6</td><td>D</td></tr><tr><td>7</td><td>8</td><td>9</td><td>E</td></tr><tr><td>A</td><td>0</td><td>B</td><td>F</td></tr></tbody></table>

#### Mapped QWERTY Keys
<table><tbody><tr><td>1</td><td>2</td><td>3</td><td>4</td></tr><tr><td>Q</td><td>W</td><td>E</td><td>R</td></tr><tr><td>A</td><td>S</td><td>D</td><td>F</td></tr><tr><td>Z</td><td>X</td><td>C</td><td>V</td></tr></tbody></table>

## How to use
```batch
CHIP8Emulator.exe roms\pong2.rom
```

The clock speed of CHIP-8 is not formally defined, and it seems that the clock speed changed depending on which computer the game was intended to run on.
You can set the simulated clock speed at the `#define CLOCK_SPEED_HZ 540` at the top of `main.cpp`.
If the game is running too slowly, try increasing the clock speed. If the game is missing keyboard input, try decreasing the clock speed.

## TODO
- Sound output currently does not work (The sound register does decrement every 1/60 of a second, but no tone is heard)
    It seems that on Windows if I want to play sound with full control of timing I need to use quite a bit code if I don't
    want to use a library. I need to find the simplest path to interface with Windows.
- I should probably add an option for the user to set the simulated clock HZ, as different games assume different clock speeds.
- Implement Super-Chip8 ISA extensions.

## References
- The code is mainly based on [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).
- The Windows bindings are based on the first few episodes of [Casey Muratori's Handmade Hero](https://guide.handmadehero.org) series.
- I tested my emulator using ROMs from [badlogic's repo](https://github.com/badlogic/chip8/tree/master/roms).
