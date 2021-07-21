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

    uint8_t keyinput[NUM_KEYS] {};
    uint32_t videoOutput[VID_WIDTH * VID_HEIGHT] {};

    uint8_t delay_timer {};
    uint8_t audio_timer {};

    uint16_t pc {}; // Program counter for memory
    uint8_t sp {}; // Stack pointer
    uint16_t index {};
    uint16_t opcode {};
    
    std::default_random_engine gen_rand;
	  std::uniform_int_distribution<uint8_t> rand_byte;

    // OPcode method matcher
    void match_opcodes(uint16_t opcode);
    
    // OPcode handlers
    void OP_NULL();     // Do nothing
    void OP_00E0();     // CLS
    void OP_00EE();     // RET
    void OP_1nnn();     // JP address
    void OP_2nnn();     // CALL address
    void OP_3xkk();     // SE Vx, byte
    void OP_4xkk();     // SNE Vx, byte
    void OP_5xy0();     // SE Vx, Vy
    void OP_6xkk();     // LD Vx, byte
    void OP_7xkk();     // ADD Vx, byte
    void OP_8xy0();     // LD Vx, Vy
    void OP_8xy1();     // OR Vx, Vy
    void OP_8xy2();     // AND Vx, Vy
    void OP_8xy3();     // XOR Vx, Vy
    void OP_8xy4();     // ADD Vx, Vy
    void OP_8xy5();     // SUB Vx, Vy
    void OP_8xy6();     // SHR Vx
    void OP_8xy7();     // SUBN Vx, Vy
    void OP_8xyE();     // SHL Vx
    void OP_9xy0();     // SNE Vx, Vy
    void OP_Annn();     // LD I, address
    void OP_Bnnn();     // JP V0, address
    void OP_Cxkk();     // RND Vx, byte
    void OP_Dxyn();     // DRW Vx, Vy, height
    void OP_Ex9E();     // SKP Vx
    void OP_ExA1();     // SKNP Vx
    void OP_Fx07();     // LD Vx, DT
    void OP_Fx0A();     // LD Vx, K
    void OP_Fx15();     // LD DT, Vx
    void OP_Fx18();     // LD ST, Vx
    void OP_Fx1E();     // ADD I, Vx
    void OP_Fx29();     // LD F, Vx
    void OP_Fx33();     // LD B, Vx
    void OP_Fx55();     // LD [I], Vx
    void OP_Fx65();     // LD Vx, [I]
};

#endif 