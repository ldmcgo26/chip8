#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>

class Chip8
{
public:
	Chip8();
	uint8_t registers[16]{};
	uint8_t memory[4096]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[16]{};
	uint32_t video[64 * 32]{};
	uint16_t opcode;
    void LoadROM(char const* filename);
	void Cycle();
	void DecodeNibble(uint16_t const opcode);
	void Decode_0(uint16_t const opcode);
	void Decode_8(uint16_t const opcode);
	void Decode_E(uint16_t const opcode);
	void Decode_F(uint16_t const opcode);
};

#endif