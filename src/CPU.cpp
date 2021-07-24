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
constexpr uint8_t fonts[FONTS_SZ] {
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

void CPU::LoadROM(char const* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		std::streampos size = file.tellg();
		char* buffer = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();
    
		for (long i = 0; i < size; ++i) {
			memory[INIT_PC_POS + i] = buffer[i];
		}

    // free
		delete[] buffer;
	}
}

void CPU::Cycle() {
  /**
   * Get opcode from memory
   * 
   * Opcodes are 16 bit but a slot in memory is 8 bit
   * Hence a single opcode takes two consecutive memory slots
   * At memory[pc], we read in the first two nibbles and shift them 2 nibbles to the left
   * We combine this with the two nibbles from memory[pc + 1] to get a single 4 nibble opcode
   */
  opcode = (memory[pc] << 8u) | memory[pc + 1];

  // set pc to next instruction
  pc += 2;

  // execute instruction
  CPU::match_opcodes();

  // Decrement timers
  if (delay_timer) --delay_timer;
  if (audio_timer) --audio_timer;
}

/**
 * debugging method
 * Outputs values and their memory locations
 */
void CPU::PrintMemory() {
  for (unsigned int i = 0; i < MEMORY_SZ - 1; i+=2) {
    if (memory[i]) {
      uint16_t curr_op = (memory[i] << 8u) | memory[i + 1];
      std::cout << std::hex << +curr_op << "\n";
    }
  }
}