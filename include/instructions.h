#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "chip8.h"

// Declarações de todas as funções de instruções CHIP-8
// Cada função recebe o opcode completo e a estrutura Chip8

// Instruções de sistema
void inst_00E0(Chip8* chip8, uint16_t opcode); // CLS - Limpar tela
void inst_00EE(Chip8* chip8, uint16_t opcode); // RET - Retornar de sub-rotina

// Instruções de salto
void inst_1NNN(Chip8* chip8, uint16_t opcode); // JP addr - Pular para endereço

// Instruções de sub-rotina
void inst_2NNN(Chip8* chip8, uint16_t opcode); // CALL addr - Chamar sub-rotina

// Instruções condicionais
void inst_3XKK(Chip8* chip8, uint16_t opcode); // SE Vx, byte - Pular se Vx == KK
void inst_4XKK(Chip8* chip8, uint16_t opcode); // SNE Vx, byte - Pular se Vx != KK
void inst_5XY0(Chip8* chip8, uint16_t opcode); // SE Vx, Vy - Pular se Vx == Vy

// Instruções de atribuição e aritmética
void inst_6XKK(Chip8* chip8, uint16_t opcode); // LD Vx, byte - Vx = KK
void inst_7XKK(Chip8* chip8, uint16_t opcode); // ADD Vx, byte - Vx += KK
void inst_8XY0(Chip8* chip8, uint16_t opcode); // LD Vx, Vy - Vx = Vy
void inst_8XY1(Chip8* chip8, uint16_t opcode); // OR Vx, Vy - Vx |= Vy
void inst_8XY2(Chip8* chip8, uint16_t opcode); // AND Vx, Vy - Vx &= Vy
void inst_8XY3(Chip8* chip8, uint16_t opcode); // XOR Vx, Vy - Vx ^= Vy
void inst_8XY4(Chip8* chip8, uint16_t opcode); // ADD Vx, Vy - Vx += Vy (com carry)
void inst_8XY5(Chip8* chip8, uint16_t opcode); // SUB Vx, Vy - Vx -= Vy (com borrow)
void inst_8XY6(Chip8* chip8, uint16_t opcode); // SHR Vx - Shift right
void inst_8XY7(Chip8* chip8, uint16_t opcode); // SUBN Vx, Vy - Vx = Vy - Vx
void inst_8XYE(Chip8* chip8, uint16_t opcode); // SHL Vx - Shift left

// Instruções de salto condicional
void inst_9XY0(Chip8* chip8, uint16_t opcode); // SNE Vx, Vy - Pular se Vx != Vy

// Instruções de índice e memória
void inst_ANNN(Chip8* chip8, uint16_t opcode); // LD I, addr - I = NNN
void inst_BNNN(Chip8* chip8, uint16_t opcode); // JP V0, addr - Pular para V0 + addr

// Instruções de teclado
void inst_EX9E(Chip8* chip8, uint16_t opcode); // SKP Vx - Pular se tecla Vx pressionada
void inst_EXA1(Chip8* chip8, uint16_t opcode); // SKNP Vx - Pular se tecla Vx não pressionada

void inst_FX07(Chip8* chip8, uint16_t opcode); // LD Vx, DT - Vx = delay_timer
void inst_FX0A(Chip8* chip8, uint16_t opcode); // LD Vx, K - Aguardar tecla
void inst_FX15(Chip8* chip8, uint16_t opcode); // LD DT, Vx - delay_timer = Vx
void inst_FX18(Chip8* chip8, uint16_t opcode); // LD ST, Vx - sound_timer = Vx
void inst_FX1E(Chip8* chip8, uint16_t opcode); // ADD I, Vx - I += Vx
void inst_FX29(Chip8* chip8, uint16_t opcode); // LD F, Vx - I = endereço do sprite
void inst_FX33(Chip8* chip8, uint16_t opcode); // LD B, Vx - Armazenar BCD
void inst_FX55(Chip8* chip8, uint16_t opcode); // LD [I], Vx - Salvar registradores
void inst_FX65(Chip8* chip8, uint16_t opcode); // LD Vx, [I] - Carregar registradores

// Instrução de desenho
void inst_DXYN(Chip8* chip8, uint16_t opcode); // DRW Vx, Vy, nibble - Desenhar sprite

#endif
