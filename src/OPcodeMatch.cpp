/**
 * Contains opcode handlers and matcher
 */
#include <cstdint>
#include <string.h>
#include <iostream>
#include <iomanip>
#include "CPU.h"

// Null Opcode
void CPU::OP_NULL() {
  std::cout << "RAN FOR: " << std::hex << opcode << "\n";
  return;
}

// Clear the screen
void CPU::OP_00E0() {
  memset(videoOutput, 0, sizeof(videoOutput));
}

/**
 * Returns to call position
 * by getting the pc's position from
 * the stack when it was called
 */
void CPU::OP_00EE() {
  --sp;
  pc = stack[sp];
}

// Jumps to position nnn
void CPU::OP_1nnn() {
  // 0FFF -> 0000 1111 1111 1111
  // jumps to nnn position
  pc = (opcode & 0x0FFFu);
}

/**
 * Jumps to a new position
 * but remember the original
 * called position so we can return
 */
void CPU::OP_2nnn() {
  stack[sp] = pc; // place current position on top of stack
  ++sp; 
  pc = (opcode & 0x0FFFu); // jump to new pos
}

/**
 * Acts as an if statement for equivalence with a literal
 * skips the next instruction if Vx = kk
 * Vx stands for register x
 */
void CPU::OP_3xkk() {
  // get x from 3xkk and shift it to the first nibble
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  uint8_t kk = opcode & 0x00FFu;
  if (registers[Vx] == kk) pc += 2;
}

/**
 * Acts as an if statement for inequivalence with a literal
 * skips the next instruction if Vx = kk
 */
void CPU::OP_4xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t kk = opcode & 0x00FFu;
  if (registers[Vx] != kk) pc += 2;
}

/**
 * Acts as an if statement for equivalence with a another register
 * skips the next instruction if Vx = Vy
 */
void CPU::OP_5xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u; // register x
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; // register y
  if (registers[Vx] == registers[Vy]) pc += 2;
}

/**
 * Set register x to kk
 * Vx = kk
 */
void CPU::OP_6xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;
	registers[Vx] = kk;
}

/**
 * Add kk to register x
 * Vx += kk
 */
void CPU::OP_7xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;
	registers[Vx] += kk;
}

/**
 * Let register x = register y
 * Vx = Vy
 */
void CPU::OP_8xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vy];
}

// Vx = Vx OR Vy
void CPU::OP_8xy1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] |= registers[Vy];
}

// Vx = Vx AND Vy
void CPU::OP_8xy2() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] &= registers[Vy];
}

// Vx = Vx XOR Vy
void CPU::OP_8xy3() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] ^= registers[Vy];
}

/**
 * Vx = Vx + Vy
 * Overflow flag at 0xF (register 15)
 * Stores first 2 nibbles (8 bits)
 */
void CPU::OP_8xy4() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint16_t sum = registers[Vx] + registers[Vy];
  
  registers[0xF] = sum > 255U ? 1 : 0; // set overflow flag
  registers[Vx] = (sum & 0xFFu); // get 2 nibbles
}

/**
 * Vx = Vx - Vy
 * Flag indicating Vx > vy
 */
void CPU::OP_8xy5() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  registers[0xF] = registers[Vx] > registers[Vy] ? 1 : 0; // set flag
  registers[Vx] -= registers[Vy]; 
}

/**
 * Vx /= 2
 * Flag indicating division by two remainder
 */
void CPU::OP_8xy6() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  // only need to check least signifiant bit for parity
	registers[0xF] = (registers[Vx] & 0x1u); 
	registers[Vx] >>= 1;
}

/**
 * Vx = Vy - Vx
 * Flag indicating Vy > vx
 */
void CPU::OP_8xy7() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
  registers[0xF] = registers[Vy] > registers[Vx] ? 1 : 0; // set flag
  registers[Vx] = registers[Vy] - registers[Vx]; 
}

/**
 * Vx *= 2
 * Flag for overflow
 */
void CPU::OP_8xyE() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  // check most significant bit for overflow
  registers[0xFu] = (registers[Vx] & 0x80u) >> 7u; 
  registers[Vx] <<= 1; // multiply by 2
}

/**
 * Skip next instruction if Vx != Vy
 */
void CPU::OP_9xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] != registers[Vy]) pc += 2;
}

/**
 * Store nnn in the index
 */
void CPU::OP_Annn() {
  index = (opcode & 0x0FFFu);
}

/**
 * Jumps to location nnn + V0 (register 0)
 */
void CPU::OP_Bnnn() {
  pc = registers[0] + (opcode & 0x0FFFu);
}

/**
 * Stores random 8 bit AND kk in Vx
 */
void CPU::OP_Cxkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = opcode & 0x00FFu;
	registers[Vx] = rand_byte(gen_rand) & kk;
}

/**
 * Draws an n-height sprite starting from (Vx, Vy) and saving in memory[index]
 * Flag for pixel collision 
 */
void CPU::OP_Dxyn() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

  // wrap when overflows screen
  uint8_t xPos = registers[Vx] % VID_WIDTH;
	uint8_t yPos = registers[Vy] % VID_HEIGHT;
  //std::cout << std::hex << +registers[Vx] << " " << +registers[Vy] << "\n";
  // default no collision
  registers[0xFu] = 0;
  for (unsigned int row = 0; row < height; ++row) {
    // memory can only store 8 bits, so there is at most
    // 8 columns stored in memory for a row
    uint8_t pixels = memory[index + row];
    for (unsigned int col = 0; col < 8; ++col) {
      // 0x80 = 1000 0000
      // shift it by col to get all 8 pixels
      uint8_t pixel = pixels & (0x80u >> col);
      
      // Determine if the pixel is already in use
      if (pixel) {
        // reference to pixel on actual screen
        uint32_t& screenPixel = videoOutput[(yPos + row) * VID_WIDTH + (xPos + col)];
        // Account for collision
        if (screenPixel == 0xFFFFFFFF) registers[0xFu] = 1;
        // XOR the pixel to make the change
        screenPixel ^= 0xFFFFFFFF;
      }
    } 
  }
}

/**
 * Skip instruction if Vx is equal to the key pressed
 */
void CPU::OP_Ex9E() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if (keyInput[key]) pc += 2;
}

/**
 * Skip instruction if Vx is equal to the key is not pressed
 */
void CPU::OP_ExA1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if (!keyInput[key]) pc += 2;
}

/**
 * Vx = delay timer
 */
void CPU::OP_Fx07() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delay_timer;
}

/**
 * Wait for a key press and store it in Vx
 */
void CPU::OP_Fx0A() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (unsigned int i = 0; i < 16; ++i) {
    if (keyInput[i]) {
      registers[Vx] = i;
      return;
    }
  }
  pc -= 2; // Keep repeating this instruction until we get a key
}

// Set delay timer
void CPU::OP_Fx15() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delay_timer = registers[Vx];
}

// Set audio timer
void CPU::OP_Fx18() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	audio_timer = registers[Vx];
}

// Add Vx to index
void CPU::OP_Fx1E() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += registers[Vx];
}

// Index = location of sprite for digit
void CPU::OP_Fx29() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];
	index = 0x50u + (5 * digit);
}

/**
 * store a three digit decimal using 3 consecutive memory locations
 * index, index + 1, index + 2
 */
void CPU::OP_Fx33() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

  for (int i = 2; i >= 0; --i) {
    memory[index + i] = value % 10;
    value /= 10;
  }
}

/**
 * Store registers V0...Vx starting at memory[index]
 */
void CPU::OP_Fx55() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; ++i) {
		memory[index + i] = registers[i];
	}
}

/**
 * Read registers V0...Vx from memory[index]
 */
void CPU::OP_Fx65() {
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; ++i) {
		registers[i] = memory[index + i];
	}
}

/**
 * Switch statement that matches an opcode with its corresponding 
 * method handler
 */
void CPU::match_8opcodes() {
  uint8_t endNibble = (opcode & 0x000Fu);
      // std::cout << std::hex << opcode << " End " << +endNibble << "\n";

  switch (endNibble) {
    case 0x0:
      CPU::OP_8xy0();
      break;
    case 0x1:
      CPU::OP_8xy1();
      break;
    case 0x2:
      CPU::OP_8xy2();
      break;
    case 0x3:
      CPU::OP_8xy3();
      break;
    case 0x4:
      CPU::OP_8xy4();
      break;
    case 0x5:
      CPU::OP_8xy5();
      break;
    case 0x6:
      CPU::OP_8xy6();
      break;
    case 0x7:
      CPU::OP_8xy7();
      break;
    case 0xE:
      CPU::OP_8xyE();
      break;
    default:
      CPU::OP_NULL();
      break;
  }
}
void CPU::match_0opcodes() {
  uint8_t endNibble = (opcode & 0x000Fu);
    // std::cout << std::hex << opcode << " End " << +endNibble << "\n";
  switch (endNibble) {
    case 0x0:
      CPU::OP_00E0();
      break;
    case 0xE:
      CPU::OP_00EE();
      break;
    default:
      CPU::OP_NULL();
      break;
  }
}
void CPU::match_Eopcodes() {
    uint8_t lastTwoNibbles = (opcode & 0x00FFu);
      // std::cout << std::hex << opcode << " LASTTWO " << +lastTwoNibbles << "\n";
    switch (lastTwoNibbles) {
      case 0xA1:
        CPU::OP_ExA1();
        break;
      case 0x9E:
        CPU::OP_Ex9E();
        break;
      default:
        CPU::OP_NULL();
        break;
    }
}
void CPU::match_Fopcodes() {
  uint8_t lastTwoNibbles = (opcode & 0x00FFu);
  // std::cout << std::hex << opcode << " LASTTWO " << +lastTwoNibbles << "\n";
  switch (lastTwoNibbles) {
    case 0x07:
      CPU::OP_Fx07();
      break;
    case 0x0A:
      CPU::OP_Fx0A();
      break;
    case 0x15:
      CPU::OP_Fx15();
      break;
    case 0x18:
      CPU::OP_Fx18();
      break;
    case 0x1E:
      CPU::OP_Fx1E();
      break;
    case 0x29:
      CPU::OP_Fx29();
      break;
    case 0x33:
      CPU::OP_Fx33();
      break;
    case 0x55:
      CPU::OP_Fx55();
      break;
    case 0x65:
      CPU::OP_Fx65();
      break;
    default:
      CPU::OP_NULL();
      break;
  }
}

void CPU::match_opcodes() {
  uint8_t frontNibble = (opcode & 0xF000u) >> 12u;
  // std::cout << std::hex << opcode << " MAIN " << +frontNibble << "\n";
  switch (frontNibble) {
    case 0x0: 
      CPU::match_0opcodes();
      break;
    case 0x1: 
      CPU::OP_1nnn();
      break;
    case 0x2:
      CPU::OP_2nnn();
      break;
    case 0x3:
      CPU::OP_3xkk();
      break;
    case 0x4:
      CPU::OP_4xkk();
      break;
    case 0x5:
      CPU::OP_5xy0();
      break;
    case 0x6:
      CPU::OP_6xkk();
      break;
    case 0x7:
      CPU::OP_7xkk();
      break;
    case 0x8:
      CPU::match_8opcodes();
      break;
    case 0x9:
      CPU::OP_9xy0();
      break;
    case 0xA:
      CPU::OP_Annn();
      break;
    case 0xB:
      CPU::OP_Bnnn();
      break;
    case 0xC:
      CPU::OP_Cxkk();
      break;
    case 0xD:
      CPU::OP_Dxyn();
      break;
    case 0xE:
      CPU::match_Eopcodes();
      break;
    case 0xF:
      CPU::match_Fopcodes();
      break;
    default:
      CPU::OP_NULL();
  }
}