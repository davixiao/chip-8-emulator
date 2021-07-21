#ifndef CPU_H 
#define CPU_H

#include <cstdint>
#include <random>

class CPU {
  public:
    CPU();
    void LoadROM(char const* file);
    void Cycle();
    void PrintMemory();
  private:
    // Class Constants
    static constexpr unsigned int MEMORY_SZ {4096};
    static constexpr unsigned int STACK_SZ {16};
    static constexpr unsigned int NUM_REGISTERS {16};

    static constexpr unsigned int VID_HEIGHT {16};
    static constexpr unsigned int VID_WIDTH {16};
    static constexpr unsigned int NUM_KEYS {16};

    // CPU Variables
    uint8_t memory[MEMORY_SZ] {};
    uint8_t registers[NUM_REGISTERS] {};
    uint8_t stack[STACK_SZ] {};

    uint8_t delay_timer {};
    uint8_t audio_timer {};

    uint16_t pc {}; // Program counter for memory
    uint8_t sp {}; // Stack pointer
    uint16_t index {};
    uint16_t opcode {};
    
    std::default_random_engine gen_rand;
	  std::uniform_int_distribution<uint8_t> rand_byte;
};

#endif 