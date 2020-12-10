#pragma once

// Based on Cowgod's Chip-8 Technical Reference (http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
/*
Keyboard mapping:

1 2 3 C      1 2 3 4
4 5 6 D  ->  Q W E R
7 8 9 E      A S D F
A 0 B F      Z X C V
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

#define MEM_SIZE 4096
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
typedef unsigned char byte;

class Chip8 {
	const int screen_width = 64;

	// The first 512 bytes of memory are reserved for the interpreter,
	// we only use them to store font sprites.
	byte memory[MEM_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // "0"
		0x20, 0x60, 0x20, 0x20, 0x70, // "1"
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // "2"
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // "3"
		0x90, 0x90, 0xF0, 0x10, 0x10, // "4"
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // "5"
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // "6"
		0xF0, 0x10, 0x20, 0x40, 0x40, // "7"
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // "8"
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // "9"
		0xF0, 0x90, 0xF0, 0x90, 0x90, // "A"
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // "B"
		0xF0, 0x80, 0x80, 0x80, 0xF0, // "C"
		0xE0, 0x90, 0x90, 0x90, 0xE0, // "D"
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // "E"
		0xF0, 0x80, 0xF0, 0x80, 0x80, // "F"
	};
	short stack[16] = {};

	byte V_registers[16] = {};
	short I_register = 0;

	short PC_register = 512;
	byte SP_register = -1; // The sp register is the top of the currently occupied stack: -1 means the stack is empty.
	byte DT_register = 0;
	byte ST_register = 0;

	byte screen[SCREEN_WIDTH * SCREEN_HEIGHT] = {};
	bool screen_dirty = false;

	bool blocking_for_key = false;
public:
	// Construct CHIP8 interpreter with a rom file loaded into memory
	Chip8(const char* rom_filename);
	// Execute one instruction
	void step();
	// Decrement the clock registers. Should be called 60 times a second.
	void step_clocks();
	// Whether or not the screen needs to be redrawn because of the last step
	bool is_screen_dirty() const;
	// Whether or not the specified pixel in the screen is turned on
	bool get_pixel_value(int x, int y) const;
private:
	void instr_0nnn(short instr);
	void instr_00E0(short instr);
	void instr_00EE(short instr);
	void instr_1nnn(short instr);
	void instr_2nnn(short instr);
	void instr_3xkk(short instr);
	void instr_4xkk(short instr);
	void instr_5xy0(short instr);
	void instr_6xkk(short instr);
	void instr_7xkk(short instr);
	void instr_8xy0(short instr);
	void instr_8xy1(short instr);
	void instr_8xy2(short instr);
	void instr_8xy3(short instr);
	void instr_8xy4(short instr);
	void instr_8xy5(short instr);
	void instr_8xy6(short instr);
	void instr_8xy7(short instr);
	void instr_8xyE(short instr);
	void instr_9xy0(short instr);
	void instr_Annn(short instr);
	void instr_Bnnn(short instr);
	void instr_Cxkk(short instr);
	void instr_Dxyn(short instr);
	void instr_Ex9E(short instr);
	void instr_ExA1(short instr);
	void instr_Fx07(short instr);
	void instr_Fx0A(short instr);
	void instr_Fx15(short instr);
	void instr_Fx18(short instr);
	void instr_Fx1E(short instr);
	void instr_Fx29(short instr);
	void instr_Fx33(short instr);
	void instr_Fx55(short instr);
	void instr_Fx65(short instr);
};
