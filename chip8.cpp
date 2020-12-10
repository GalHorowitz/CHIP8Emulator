#include "chip8.h"
#include "windows_bindings.h"

Chip8::Chip8(const char* rom_filename) {
	// Read rom into memory
	std::ifstream rom_file(rom_filename, std::ios::in | std::ios::binary);
	if (!rom_file.is_open()) {
		throw std::runtime_error("Failed to open rom file");
	}
	rom_file.read((char*)&memory[512], MEM_SIZE - 512);
	rom_file.close();
}

bool Chip8::is_screen_dirty() const {
	return screen_dirty;
}

bool Chip8::get_pixel_value(int x, int y) const {
	return screen[y * SCREEN_WIDTH + x];
}

void Chip8::step_clocks() {
	if (DT_register > 0) {
		DT_register--;
	}

	if (ST_register > 0) { // TODO: Sound
		//std::cerr << "Sound is not implemented" << std::endl;
		ST_register--;
	}
}

void Chip8::step() {
	// Reset the dirty state, appropriate instructions will set it.
	screen_dirty = false;

	// An instruction is 2 bytes long, big-endian
	short instr = ((memory[PC_register] << 8) & 0xFF00 | (memory[PC_register + 1]));

	if (PC_register + 1 > MEM_SIZE) {
		throw std::runtime_error("Executing instructions out of bounds");
	}

	// Decode instruction
	switch ((memory[PC_register] >> 4) & 0xF) {
	case 0x0: {
		if (instr == 0x00E0) {
			instr_00E0(instr);
		} else if (instr == 0x00EE) {
			instr_00EE(instr);
		} else {
			instr_0nnn(instr);
		}
	} break;
	case 0x1: {
		instr_1nnn(instr);
	} break;
	case 0x2: {
		instr_2nnn(instr);
	} break;
	case 0x3: {
		instr_3xkk(instr);
	} break;
	case 0x4: { 
		instr_4xkk(instr);
	} break;
	case 0x5: { 
		instr_5xy0(instr);
	} break;
	case 0x6: { 
		instr_6xkk(instr);
	} break;
	case 0x7: { 
		instr_7xkk(instr);
	} break;
	case 0x8: {
		switch (instr & 0xF) {
		case 0x0: { 
			instr_8xy0(instr);
		} break;
		case 0x1: { 
			instr_8xy1(instr);
		} break;
		case 0x2: { 
			instr_8xy2(instr);
		} break;
		case 0x3: { 
			instr_8xy3(instr);
		} break;
		case 0x4: { 
			instr_8xy4(instr);
		} break;
		case 0x5: { 
			instr_8xy5(instr);
		} break;
		case 0x6: { 
			instr_8xy6(instr);
		} break;
		case 0x7: { 
			instr_8xy7(instr);
		} break;
		case 0xE: { 
			instr_8xyE(instr);
		} break;
		default: {
			throw std::runtime_error("Unknown 8xy? instruction.");
			//std::cerr << "Unknown instruction: " << memory[PC_register] << ", " << memory[PC_register + 1] << std::endl;
		} break;
		}
	} break;
	case 0x9: { 
		instr_9xy0(instr);
	} break;
	case 0xA: { 
		instr_Annn(instr);
	} break;
	case 0xB: { 
		instr_Bnnn(instr);
	} break;
	case 0xC: { 
		instr_Cxkk(instr);
	} break;
	case 0xD: { 
		instr_Dxyn(instr);
	} break;
	case 0xE: {
		switch (memory[PC_register + 1]) {
		case 0x9E: { 
			instr_Ex9E(instr);
		} break;
		case 0xA1: { 
			instr_ExA1(instr);
		} break;
		default: {
			throw std::runtime_error("Unknown Ex?? instruction.");
			//std::cerr << "Unknown instruction: " << memory[PC_register] << ", " << memory[PC_register + 1] << std::endl;
		} break;
		}
	} break;
	case 0xF: {
		switch (memory[PC_register + 1]) {
		case 0x07: { 
			instr_Fx07(instr);
		} break;
		case 0x0A: { 
			instr_Fx0A(instr);
		} break;
		case 0x15: { 
			instr_Fx15(instr);
		} break;
		case 0x18: { 
			instr_Fx18(instr);
		} break;
		case 0x1E: { 
			instr_Fx1E(instr);
		} break;
		case 0x29: { 
			instr_Fx29(instr);
		} break;
		case 0x33: { 
			instr_Fx33(instr);
		} break;
		case 0x55: { 
			instr_Fx55(instr);
		} break;
		case 0x65: { 
			instr_Fx65(instr);
		} break;
		default: {
			throw std::runtime_error("Unknown Fx?? instruction.");
			//std::cerr << "Unknown instruction: " << memory[PC_register] << ", " << memory[PC_register + 1] << std::endl;
		} break;
		}
	} break;
	default: {
		throw std::runtime_error("Unknown instruction.");
		//std::cerr << "Unknown instruction: " << memory[PC_register] << ", " << memory[PC_register + 1] << std::endl;
	} break;
	}

	PC_register += 2;
	//PC_register = (PC_register) % MEM_SIZE; // Normalize PC
}

#define ADDR(instr) ((instr)&0xFFF)
#define X_REG(instr) ((instr>>8)&0xF)
#define Y_REG(instr) ((instr>>4)&0xF)
#define IMM_BYTE(instr) ((instr)&0xFF)
#define IMM_NIBBLE(instr) ((instr)&0xF)

// SYS addr
void Chip8::instr_0nnn(short instr) {
	// Instruction ignored.
	std::cout << "SYS instruction with address: " << ADDR(instr) << std::endl;
}

// CLS
void Chip8::instr_00E0(short instr) {
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			screen[y * SCREEN_WIDTH + x] = 0;
		}
	}
	screen_dirty = true;
}

/*
When we set PC, we have to adjust the addr to account for the fact that
we increment PC after each instruction. Relevant for JP, RET, CALL
*/

// RET
void Chip8::instr_00EE(short instr) {
	PC_register = stack[SP_register];
	if (SP_register == -1) {
		throw std::runtime_error("Too many RET instructions, nowhere to return to");
	}
	SP_register--;
}

// JP addr
void Chip8::instr_1nnn(short instr) {
	PC_register = ADDR(instr) - 2;
}

// CALL addr
void Chip8::instr_2nnn(short instr) {
	SP_register++;
	if (SP_register > 15) {
		throw std::runtime_error("CHIP-8 Only supports 16 levels of nested subroutines.");
	}
	stack[SP_register] = PC_register;
	PC_register = ADDR(instr) - 2;
}

// JP V0, addr
void Chip8::instr_Bnnn(short instr) {
	PC_register = ADDR(instr) + V_registers[0] - 2;
}

// SE Vx, byte
void Chip8::instr_3xkk(short instr) {
	if (V_registers[X_REG(instr)] == IMM_BYTE(instr)) {
		PC_register += 2;
	}
}

// SNE Vx, byte
void Chip8::instr_4xkk(short instr) {
	if (V_registers[X_REG(instr)] != IMM_BYTE(instr)) {
		PC_register += 2;
	}
}

// SE Vx, Vy
void Chip8::instr_5xy0(short instr) {
	if (V_registers[X_REG(instr)] == V_registers[Y_REG(instr)]) {
		PC_register += 2;
	}
}

// LD Vx, byte
void Chip8::instr_6xkk(short instr) {
	V_registers[X_REG(instr)] = IMM_BYTE(instr);
}

// ADD Vx, byte
void Chip8::instr_7xkk(short instr) {
	V_registers[X_REG(instr)] += IMM_BYTE(instr);
}

// LD Vx, Vy
void Chip8::instr_8xy0(short instr) {
	V_registers[X_REG(instr)] = V_registers[Y_REG(instr)];
}

// OR Vx, Vy
void Chip8::instr_8xy1(short instr) {
	V_registers[X_REG(instr)] |= V_registers[Y_REG(instr)];
}

// AND Vx, Vy
void Chip8::instr_8xy2(short instr) {
	V_registers[X_REG(instr)] &= V_registers[Y_REG(instr)];
}

// XOR Vx, Vy
void Chip8::instr_8xy3(short instr) {
	V_registers[X_REG(instr)] ^= V_registers[Y_REG(instr)];
}

// ADD Vx, Vy
void Chip8::instr_8xy4(short instr) {
	short sum = ((short)V_registers[X_REG(instr)]) + ((short)V_registers[Y_REG(instr)]);
	V_registers[0xF] = (sum > 255) ? 1 : 0; // Overflow
	V_registers[X_REG(instr)] = sum & 0xFF;
}

// SUB Vx, Vy
void Chip8::instr_8xy5(short instr) {
	V_registers[0xF] = (V_registers[X_REG(instr)] >= V_registers[Y_REG(instr)]) ? 1 : 0;
	V_registers[X_REG(instr)] -= V_registers[Y_REG(instr)];
}

/*
	According to wikipedia, in the original CHIP-8 interpreter opcodes 8XY6, 8XYE
	were undocument opcodes which shifted Vy and stored the result in Vx.
	Instead, modern interpreters chose to ignore Vy and just shift Vx. I am
	conforming to the later standard because I assume the roms I have available
	were written with that standard in mind.
*/

// SHR Vx {, Vy}
void Chip8::instr_8xy6(short instr) {
	V_registers[0xF] = V_registers[X_REG(instr)] & 1;
	V_registers[X_REG(instr)] >>= 1;
}

// SHL Vx {, Vy}
void Chip8::instr_8xyE(short instr) {
	// See SHR note
	V_registers[0xF] = (V_registers[X_REG(instr)] >> 7) & 1;
	V_registers[X_REG(instr)] <<= 1;
}

// SUBN Vx, Vy
void Chip8::instr_8xy7(short instr) {
	V_registers[0xF] = (V_registers[Y_REG(instr)] >= V_registers[X_REG(instr)]) ? 1 : 0;
	V_registers[X_REG(instr)] = V_registers[Y_REG(instr)] - V_registers[X_REG(instr)];
}

// SNE Vx, Vy
void Chip8::instr_9xy0(short instr) {
	if (V_registers[X_REG(instr)] != V_registers[Y_REG(instr)]) {
		PC_register += 2;
	}
}

// LD I, addr
void Chip8::instr_Annn(short instr) {
	I_register = ADDR(instr);
}

// RND Vx, byte
void Chip8::instr_Cxkk(short instr) {
	V_registers[X_REG(instr)] = (rand() % 256) & IMM_BYTE(instr);
}

// DRW Vx, Vy, nibble
void Chip8::instr_Dxyn(short instr) {
	int base_x = V_registers[X_REG(instr)];
	int base_y = V_registers[Y_REG(instr)];
	for (int y = 0; y < IMM_NIBBLE(instr); y++) {
		for (int x = 0; x < 8; x++) {
			int pixel_value = (memory[I_register + y] >> (7 - x)) & 1;
			int pos_x = (base_x + x) % SCREEN_WIDTH;
			int pos_y = (base_y + y) % SCREEN_HEIGHT;
			int pixel_index = pos_y * SCREEN_WIDTH + pos_x;

			if (screen[pixel_index]) {
				V_registers[0xF] = 1;
			}

			screen[pixel_index] ^= pixel_value;
		}
	}

	screen_dirty = true;
}

// SKP Vx
void Chip8::instr_Ex9E(short instr) {
	if (is_key_down(V_registers[X_REG(instr)])) {
		PC_register += 2;
	}
}

// SKNP Vx
void Chip8::instr_ExA1(short instr) {
	if (!is_key_down(V_registers[X_REG(instr)])) {
		PC_register += 2;
	}
}

// LD Vx, DT
void Chip8::instr_Fx07(short instr) {
	V_registers[X_REG(instr)] = DT_register;
}

// LD Vx, K
void Chip8::instr_Fx0A(short instr) {
	if (blocking_for_key) {
		int key_number = get_capture_key();
		if (key_number != -1) {
			V_registers[X_REG(instr)] = key_number;
			blocking_for_key = false;
		} else {
			PC_register -= 2;
		}
	} else {
		blocking_for_key = true;
		enable_key_capture();
		// We block by executing this instruction repeatedly until we capture a key press
		PC_register -= 2; 
	}
}

// LD DT, Vx
void Chip8::instr_Fx15(short instr) {
	DT_register = V_registers[X_REG(instr)];
}

// LD ST, Vx
void Chip8::instr_Fx18(short instr) {
	ST_register = V_registers[X_REG(instr)];
}

// ADD I, Vx
void Chip8::instr_Fx1E(short instr) {
	I_register += V_registers[X_REG(instr)];
}

// LD F, Vx
void Chip8::instr_Fx29(short instr) {
	// We store the font at the addr 0, and each sprite takes up 5 bytes.
	I_register = 5 * V_registers[X_REG(instr)];
}

// LD B, Vx
void Chip8::instr_Fx33(short instr) {
	if (I_register + 2 >= MEM_SIZE) {
		throw std::runtime_error(" LD B, Vx writes out of bounds");
	}
	memory[I_register] = V_registers[X_REG(instr)] / 100;
	memory[I_register + 1] = (V_registers[X_REG(instr)] / 10) % 10;
	memory[I_register + 2] = V_registers[X_REG(instr)] % 10;
}

// LD [I], Vx
void Chip8::instr_Fx55(short instr) {
	if (I_register + X_REG(instr) >= MEM_SIZE) {
		throw std::runtime_error("LD [I], Vx writes out of bounds");
	}
	for (int i = 0; i <= X_REG(instr); i++) {
		memory[I_register + i] = V_registers[i];
	}
}

// LD Vx, [I]
void Chip8::instr_Fx65(short instr) {
	if (I_register + X_REG(instr) >= MEM_SIZE) {
		throw std::runtime_error("LD Vx, [I] reads out of bounds");
	}
	for (int i = 0; i <= X_REG(instr); i++) {
		V_registers[i] = memory[I_register + i];
	}
}