/**
 * Contains opcode handlers and matcher
 */
#include "CPU.h"
#include <cstdint>
#include <string.h>
#include <iostream>

// Null Opcode
void CPU::OP_NULL() {
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
 * Switch statement that matches an opcode with its corresponding 
 * method handler
 */
void CPU::match_opcodes(uint16_t opcode) {
  std::cout << opcode << "\n";
}