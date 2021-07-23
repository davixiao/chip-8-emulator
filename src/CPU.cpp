#include "CPU.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <random>
#include <iostream>
#include <iomanip>
// Store initial position for pc
constexpr unsigned int INIT_PC_POS = 0x200;

constexpr unsigned int FONTS_SZ = 80;
constexpr unsigned int INIT_FONT_POS = 0x50;
constexpr uint8_t fonts[FONTS_SZ] 
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

// CPU Initial State
CPU::CPU() 
  : gen_rand(std::chrono::system_clock::now().time_since_epoch().count()),
    pc {INIT_PC_POS},
    // Generate random byte between 0 and 255
    rand_byte {std::uniform_int_distribution<uint8_t>(0, 255U)}     
{  
  // Initialize fonts in memory
  for (unsigned int i = 0; i < FONTS_SZ; ++i) {
    memory[INIT_FONT_POS + i] = fonts[i];
  }
}

void CPU::Cycle() {
  // get opcode from memory
  opcode = (memory[pc] << 8u) | memory[pc + 1];
  // std::cout << std::hex << opcode << "\n";
  // set pc to next instruction
  pc += 2;

  // execute instruction
  CPU::match_opcodes();

  // Decrement timers
  if (delay_timer) --delay_timer;
  if (audio_timer) --audio_timer;
}

void CPU::LoadROM(char const* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		char* buffer = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (long i = 0; i < size; ++i)
		{
			memory[INIT_PC_POS + i] = buffer[i];
		}

		delete[] buffer;
	}
}


/**
 * debugging method
 * Outputs values and their memory locations
 */
void CPU::PrintMemory() {
  for (unsigned int i = 0; i < MEMORY_SZ - 1; i+=2) {
    if (memory[i]) {
      // opcodes are 16 bit but each memory 
      // slot is 8 bit. We need to use 2 slots.
      // for hex 9842 it is split into 98 in
      /**
       * the first slot and 42 in the second. In
       * binary that is:
       * 
       * 98 Hex: 1001 1000
       * 42 Hex: 0100 0010
       * 
       * We then shift the 98 since it should
       * be in front
       * 
       * 1001 1000 0000 0000
       * OR to combine
       * 1001 1000 0000 0000 | 0100 0010
       * 1001 1000 0100 0010
       * 
       * when translated back to hex is: 9842
       */
      int lol = (memory[i] << 8u) | memory[i + 1];
      /**
       * 0F000 hex
       * 1111 0000 0000 0000
       * and with this will return the first 
       * the first nibble (4 bits) is returned.
       * so from 9842, we get 9000
       * 1001 0000 0000 0000
       * shift it right by 12 bits
       * 0000 0000 0000 1001
       * we get 9.
       */
      int troll = (lol & 0xF000u) >> 12u;
      //std::cout << i << " " << troll << "\n";
    }
  }
}