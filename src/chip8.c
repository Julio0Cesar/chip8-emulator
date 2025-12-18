#include "chip8.h"
#include "instructions.h"
#include <stdio.h>
#include <string.h>

// Fonte CHIP-8: cada caractere (0-F) tem 5 bytes, total de 80 bytes
static const uint8_t font[80] = {
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
  0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void chip8_init(Chip8* chip8) {
  // Zera toda a memória
  memset(chip8->memory, 0, sizeof(chip8->memory));

  // Zera todos os registradores V[0-15]
  memset(chip8->V, 0, sizeof(chip8->V));

  // Inicializa registradores
  chip8->I = 0;              // Registrador de índice
  chip8->pc = 0x200;         // Contador de programa começa em 0x200 (onde os programas são carregados)
  chip8->sp = 0;             // Ponteiro de pilha

  // Zera pilha, display e teclado
  memset(chip8->stack, 0, sizeof(chip8->stack));
  memset(chip8->display, 0, sizeof(chip8->display));
  memset(chip8->keypad, 0, sizeof(chip8->keypad));

  // Zera temporizadores
  chip8->delay_timer = 0;
  chip8->sound_timer = 0;

  // Carrega a fonte na memória (endereços 0x050-0x09F)
  // Cada caractere ocupa 5 bytes, então 16 caracteres × 5 = 80 bytes
  for (int i = 0; i < 80; i++) {
    chip8->memory[0x050 + i] = font[i];
  }
}

int chip8_load_rom(Chip8* chip8, const char* path) {
  FILE* file = fopen(path, "rb");
  if (!file) {
    return -1; // Arquivo não encontrado
  }

  // Descobre o tamanho do arquivo
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  // Verifica se o arquivo não excede o tamanho máximo disponível
  // Memória total: 4096 bytes, programas começam em 0x200 (512 bytes)
  // Espaço disponível: 4096 - 512 = 3584 bytes
  if (size > (4096 - 0x200)) {
    fclose(file);
    return -2; // ROM muito grande
  }

  // Lê o arquivo binário diretamente na memória começando em 0x200
  size_t bytes_read = fread(&chip8->memory[0x200], 1, size, file);
  fclose(file);

  if (bytes_read != (size_t)size) {
    return -3; // Erro ao ler o arquivo
  }

  return 0; // Sucesso
}

void chip8_cycle(Chip8* chip8) {
  // FETCH: Lê o opcode de 16 bits da memória no endereço PC
  // CHIP-8 usa big-endian: byte alto primeiro, depois byte baixo
  uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
  chip8->pc += 2; // Incrementa PC em 2 (cada instrução tem 2 bytes)

  // DECODE: Extrai o nibble superior (primeiros 4 bits) para identificar a instrução
  // EXECUTE: Chama a função correspondente à instrução
  uint16_t instruction = opcode & 0xF000;

  switch (instruction) {
    case 0x0000:
      // Instruções 0xxx - precisa verificar os últimos bits
      if (opcode == 0x00E0) {
        inst_00E0(chip8, opcode); // CLS
      } else if (opcode == 0x00EE) {
        inst_00EE(chip8, opcode); // RET
      }
      break;

    case 0x1000:
      inst_1NNN(chip8, opcode); // JP addr
      break;

    case 0x2000:
      inst_2NNN(chip8, opcode); // CALL addr
      break;

    case 0x3000:
      inst_3XKK(chip8, opcode); // SE Vx, byte
      break;

    case 0x4000:
      inst_4XKK(chip8, opcode); // SNE Vx, byte
      break;

    case 0x5000:
      inst_5XY0(chip8, opcode); // SE Vx, Vy
      break;

    case 0x6000:
      inst_6XKK(chip8, opcode); // LD Vx, byte
      break;

    case 0x7000:
      inst_7XKK(chip8, opcode); // ADD Vx, byte
      break;

    case 0x8000: {
      // Instruções 8xxx - precisa verificar o último nibble
      uint8_t last_nibble = opcode & 0x000F;
      switch (last_nibble) {
        case 0x0: inst_8XY0(chip8, opcode); break; // LD Vx, Vy
        case 0x1: inst_8XY1(chip8, opcode); break; // OR Vx, Vy
        case 0x2: inst_8XY2(chip8, opcode); break; // AND Vx, Vy
        case 0x3: inst_8XY3(chip8, opcode); break; // XOR Vx, Vy
        case 0x4: inst_8XY4(chip8, opcode); break; // ADD Vx, Vy
        case 0x5: inst_8XY5(chip8, opcode); break; // SUB Vx, Vy
        case 0x6: inst_8XY6(chip8, opcode); break; // SHR Vx
        case 0x7: inst_8XY7(chip8, opcode); break; // SUBN Vx, Vy
        case 0xE: inst_8XYE(chip8, opcode); break; // SHL Vx
      }
      break;
    }

    case 0x9000:
      inst_9XY0(chip8, opcode); // SNE Vx, Vy
      break;

    case 0xA000:
      inst_ANNN(chip8, opcode); // LD I, addr
      break;

    case 0xB000:
      inst_BNNN(chip8, opcode); // JP V0, addr
      break;

    case 0xD000:
      inst_DXYN(chip8, opcode); // DRW Vx, Vy, nibble
      break;

    case 0xE000: {
      // Instruções Exxx - precisa verificar o último byte
      uint8_t last_byte = opcode & 0x00FF;
      switch (last_byte) {
        case 0x9E: inst_EX9E(chip8, opcode); break; // SKP Vx
        case 0xA1: inst_EXA1(chip8, opcode); break; // SKNP Vx
      }
      break;
    }

    case 0xF000: {
      // Instruções Fxxx - precisa verificar o último byte
      uint8_t last_byte = opcode & 0x00FF;
      switch (last_byte) {
        case 0x07: inst_FX07(chip8, opcode); break; // LD Vx, DT
        case 0x0A: inst_FX0A(chip8, opcode); break; // LD Vx, K
        case 0x15: inst_FX15(chip8, opcode); break; // LD DT, Vx
        case 0x18: inst_FX18(chip8, opcode); break; // LD ST, Vx
        case 0x1E: inst_FX1E(chip8, opcode); break; // ADD I, Vx
        case 0x29: inst_FX29(chip8, opcode); break; // LD F, Vx
        case 0x33: inst_FX33(chip8, opcode); break; // LD B, Vx
        case 0x55: inst_FX55(chip8, opcode); break; // LD [I], Vx
        case 0x65: inst_FX65(chip8, opcode); break; // LD Vx, [I]
      }
      break;
    }

    default:
      // Opcode desconhecido - ignora (alguns programas podem ter instruções não implementadas)
      break;
  }
}