#include "chip8.h"

#include <fstream>

const unsigned int START_ADDRESS = 0x200;

// 
void Chip8::LoadROM(char const* filename)
{
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}

void Chip8::DecodeNibble(uint16_t const opcode)
{
	uint8_t nibble_1 = (opcode & 0xF000u) >> 12u;

	switch (nibble_1) 
	{
		// Call 0 handler
		case 0:
			Decode_0(opcode);
			break;

		// 1nnn - JP addr
		// Jump to location nnn.
		case 1:
			uint16_t address = opcode & 0x0FFFu;
			pc = address;
			break;

		// 2nnn - CALL addr
		// Call subroutine at nnn.
		case 2:
			uint16_t address = opcode & 0x0FFFu;

			stack[sp] = pc;
			++sp;
			pc = address;
			break;

		// 3xkk - SE Vx, byte
		// Skip next instruction if Vx = kk.	
		case 3:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t byte = opcode & 0x00FFu;

			if (registers[Vx] == byte)
			{
				pc += 2;
			}
			break;
		
		// 4xkk - SNE Vx, byte
		// Skip next instruction if Vx != kk.
		case 4:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t byte = opcode & 0x00FFu;

			if (registers[Vx] != byte)
			{
				pc += 2;
			}
			break;

		// 5xy0 - SE Vx, Vy
		// Skip next instruction if Vx = Vy.
		case 5:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			if (registers[Vx] == registers[Vy])
			{
				pc += 2;
			}
			break;
		
		// 6xkk - LD Vx, byte
		// Set Vx = kk.
		case 6:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t byte = opcode & 0x00FFu;

			registers[Vx] = byte;
			break;

		// 7xkk - ADD Vx, byte
		// Set Vx = Vx + kk.	
		case 7:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t byte = opcode & 0x00FFu;

			registers[Vx] += byte;
			break;
	
		// Call 8 handler
		case 8:
			Decode_8(opcode);
			break;

		// 9xy0 - SNE Vx, Vy
		// Skip next instruction if Vx != Vy.	
		case 9:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			if (registers[Vx] != registers[Vy])
			{
				pc += 2;
			}
			break;

		// Annn - LD I, addr
		// Set I = nnn.
		case 0xa:
			uint16_t address = opcode & 0x0FFFu;

			index = address;
			break;

		// Bnnn - JP V0, addr
		// Jump to location nnn + V0.
		case 0xb:
			uint16_t address = opcode & 0x0FFFu;

			pc = registers[0] + address;
			break;

		// Cxkk - RND Vx, byte
		// Set Vx = random byte AND kk.
		case 0xc:
			// Seed the random number generator with the current time
			std::srand(std::time(0));

			// Generate a random number between 0 and 255
			u_int8_t randomNumber = std::rand() % 256;

			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t byte = opcode & 0x00FFu;

			registers[Vx] = randomNumber & byte;
			break;

		// Dxyn - DRW Vx, Vy, nibble
		// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
		case 0xd:
			break;

		case 0xe:
			Decode_E(opcode);
			break;
		case 0xf:
			Decode_F(opcode);
			break;
	}
}

void Chip8::Decode_0(uint16_t const opcode)
{
	uint8_t nibble_4 = opcode & 0x000f;

	switch (nibble_4) 
	{
		// 00E0 - CLS
		// Clear the display.
		case 0:
			memset(video, 0, sizeof(video));
			break;
		// 00EE - RET
		// Return from a subroutine.
		case 0xe:
			--sp;
			pc = stack[sp];
			break;
	}
}

void Chip8::Decode_8(uint16_t const opcode)
{
	uint8_t nibble_4 = opcode & 0x000f;
	
	switch (nibble_4) 
	{
		// 8xy0 - LD Vx, Vy
		// Set Vx = Vy.
		case 0:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			registers[Vx] = registers[Vy];
			break;

		// 8xy1 - OR Vx, Vy
		// Set Vx = Vx OR Vy.
		case 1:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			registers[Vx] |= registers[Vy];
			break;

		// 8xy2 - AND Vx, Vy
		// Set Vx = Vx AND Vy.
		case 2:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			registers[Vx] &= registers[Vy];
			break;

		// 8xy3 - XOR Vx, Vy
		// Set Vx = Vx XOR Vy.
		case 3:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			registers[Vx] ^= registers[Vy];
			break;

		// 8xy4 - ADD Vx, Vy
		// Set Vx = Vx + Vy, set VF = carry.
		case 4:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			uint16_t sum = registers[Vx] + registers[Vy];

			if (sum > 255U)
			{
				registers[0xF] = 1;
			}
			else
			{
				registers[0xF] = 0;
			}

			registers[Vx] = sum & 0xFFu;
			break;

		// 8xy5 - SUB Vx, Vy
		// Set Vx = Vx - Vy, set VF = NOT borrow.
		case 5:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			if (registers[Vx] > registers[Vy])
			{
				registers[0xF] = 1;
			}
			else
			{
				registers[0xF] = 0;
			}

			registers[Vx] -= registers[Vy];

		// 8xy6 - SHR Vx {, Vy}
		// Set Vx = Vx SHR 1.
		case 6:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;

			// Save LSB in VF
			registers[0xF] = (registers[Vx] & 0x1u);

			registers[Vx] >>= 1;
			break;

		// 8xy7 - SUBN Vx, Vy
		// Set Vx = Vy - Vx, set VF = NOT borrow.
		case 7:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;
			uint8_t Vy = (opcode & 0x00F0u) >> 4u;

			if (registers[Vy] > registers[Vx])
			{
				registers[0xF] = 1;
			}
			else
			{
				registers[0xF] = 0;
			}

			registers[Vx] = registers[Vy] - registers[Vx];
			break;

		// 8xyE - SHL Vx {, Vy}
		// Set Vx = Vx SHL 1.
		case 0xe:
			uint8_t Vx = (opcode & 0x0F00u) >> 8u;

			// Save MSB in VF
			registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

			registers[Vx] <<= 1;
			break;
	}
}

void Chip8::Decode_E(uint16_t const opcode)
{
	uint8_t nibble_34 = opcode & 0x00ff;

	switch (nibble_34) 
	{
		case 0xa1:

		case 0x9e:

	}
}

	void Chip8::Decode_F(uint16_t const opcode)
{
	uint8_t nibble_34 = opcode & 0x00ff;

	switch (nibble_34) 
	{
		case 0x07:

		case 0x0a:

		case 0x15:

		case 0x18:

		case 0x1e:

		case 0x29:

		case 0x33:

		case 0x55:

		case 0x65:

	}
}

void Chip8::Cycle()
{
	// Fetch
	// move high byte back 8 bits to the left to make room for low byte
	// use | bitwise operator to combine the two bytes
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	// Increment the PC before we execute anything
	pc += 2;

	// Decode and Execute
	DecodeNibble(opcode);

	// Decrement the delay timer if it's been set
	if (delayTimer > 0)
	{
		--delayTimer;
	}

	// Decrement the sound timer if it's been set
	if (soundTimer > 0)
	{
		--soundTimer;
	}
}

const unsigned int FONTSET_SIZE = 80;

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// placed in "reserved" memory, common convention to put fonts at 0x50
const unsigned int FONTSET_START_ADDRESS = 0x50;

Chip8::Chip8()
{
	// Initialize PC
	pc = START_ADDRESS;

	// Load fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}
}