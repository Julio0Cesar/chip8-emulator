#include "instructions.h"
#include <string.h>

// Instruções de Sistema

// 0x00E0: CLS - Limpar tela (clear screen)
void inst_00E0(Chip8* chip8, uint16_t opcode) {
  memset(chip8->display, 0, sizeof(chip8->display));
}

// 0x00EE: RET - Retornar de sub-rotina
void inst_00EE(Chip8* chip8, uint16_t opcode) {
  if (chip8->sp > 0) {
    chip8->sp--;
    chip8->pc = chip8->stack[chip8->sp];
  }
}

// Instruções de Salto

// 0x1NNN: JP addr - Pular para endereço NNN
void inst_1NNN(Chip8* chip8, uint16_t opcode) {
  uint16_t nnn = opcode & 0x0FFF;
  chip8->pc = nnn;
}

// Instruções de Sub-rotina

// 0x2NNN: CALL addr - Chamar sub-rotina no endereço NNN
void inst_2NNN(Chip8* chip8, uint16_t opcode) {
  uint16_t nnn = opcode & 0x0FFF;
  if (chip8->sp < 16) {
    chip8->stack[chip8->sp] = chip8->pc;
    chip8->sp++;
    chip8->pc = nnn;
  }
}

// Instruções Condicionais

// 0x3XKK: SE Vx, byte - Pular próxima instrução se Vx == KK
void inst_3XKK(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = opcode & 0x00FF;
  if (chip8->V[x] == kk) {
    chip8->pc += 2;
  }
}

// 0x4XKK: SNE Vx, byte - Pular próxima instrução se Vx != KK
void inst_4XKK(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = opcode & 0x00FF;
  if (chip8->V[x] != kk) {
    chip8->pc += 2;
  }
}

// 0x5XY0: SE Vx, Vy - Pular próxima instrução se Vx == Vy
void inst_5XY0(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  if (chip8->V[x] == chip8->V[y]) {
    chip8->pc += 2;
  }
}

// Instruções de Atribuição e Aritmética

// 0x6XKK: LD Vx, byte - Vx = KK
void inst_6XKK(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = opcode & 0x00FF;
  chip8->V[x] = kk;
}

// 0x7XKK: ADD Vx, byte - Vx += KK
void inst_7XKK(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t kk = opcode & 0x00FF;
  chip8->V[x] += kk;
}

// 0x8XY0: LD Vx, Vy - Vx = Vy
void inst_8XY0(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  chip8->V[x] = chip8->V[y];
}

// 0x8XY1: OR Vx, Vy - Vx |= Vy
void inst_8XY1(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  chip8->V[x] |= chip8->V[y];
}

// 0x8XY2: AND Vx, Vy - Vx &= Vy
void inst_8XY2(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  chip8->V[x] &= chip8->V[y];
}

// 0x8XY3: XOR Vx, Vy - Vx ^= Vy
void inst_8XY3(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  chip8->V[x] ^= chip8->V[y];
}

// 0x8XY4: ADD Vx, Vy - Vx += Vy (com carry em VF)
void inst_8XY4(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint16_t sum = chip8->V[x] + chip8->V[y];
  chip8->V[0xF] = (sum > 0xFF) ? 1 : 0;
  chip8->V[x] = sum & 0xFF;
}

// 0x8XY5: SUB Vx, Vy - Vx -= Vy (com borrow em VF)
void inst_8XY5(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  chip8->V[0xF] = (chip8->V[x] >= chip8->V[y]) ? 1 : 0;
  chip8->V[x] -= chip8->V[y];
}

// 0x8XY6: SHR Vx - Shift right (VF = bit menos significativo antes do shift)
void inst_8XY6(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  chip8->V[0xF] = chip8->V[x] & 0x01;
  chip8->V[x] >>= 1;
}

// 0x8XY7: SUBN Vx, Vy - Vx = Vy - Vx (com borrow em VF)
void inst_8XY7(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  chip8->V[0xF] = (chip8->V[y] >= chip8->V[x]) ? 1 : 0;
  chip8->V[x] = chip8->V[y] - chip8->V[x];
}

// 0x8XYE: SHL Vx - Shift left (VF = bit mais significativo antes do shift)
void inst_8XYE(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  chip8->V[0xF] = (chip8->V[x] & 0x80) >> 7;
  chip8->V[x] <<= 1;
}

// Instruções de Salto Condicional

// 0x9XY0: SNE Vx, Vy - Pular próxima instrução se Vx != Vy
void inst_9XY0(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  if (chip8->V[x] != chip8->V[y]) {
    chip8->pc += 2;
  }
}

// Instruções de Índice e Memória

// 0xANNN: LD I, addr - I = NNN
void inst_ANNN(Chip8* chip8, uint16_t opcode) {
  uint16_t nnn = opcode & 0x0FFF;
  chip8->I = nnn;
}

// 0xBNNN: JP V0, addr - Pular para V0 + NNN
void inst_BNNN(Chip8* chip8, uint16_t opcode) {
  uint16_t nnn = opcode & 0x0FFF;
  chip8->pc = chip8->V[0] + nnn;
}

// 0xFX07: LD Vx, DT - Vx = delay_timer
void inst_FX07(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  chip8->V[x] = chip8->delay_timer;
}

// 0xFX0A: LD Vx, K - Aguardar tecla pressionada e armazenar em Vx
// Esta instrução pausa a execução até uma tecla ser pressionada
void inst_FX0A(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t key_pressed = 0xFF;

  // Verifica se alguma tecla está pressionada
  for (int i = 0; i < 16; i++) {
    if (chip8->keypad[i]) {
      key_pressed = i;
      break;
    }
  }

  // Se uma tecla foi pressionada, armazena e continua
  // Caso contrário, não incrementa PC (executa a mesma instrução novamente)
  if (key_pressed != 0xFF) {
    chip8->V[x] = key_pressed;
  } else {
    chip8->pc -= 2; // Volta PC para executar a mesma instrução novamente
  }
}

// 0xFX15: LD DT, Vx - delay_timer = Vx
void inst_FX15(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  chip8->delay_timer = chip8->V[x];
}

// 0xFX18: LD ST, Vx - sound_timer = Vx
void inst_FX18(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  chip8->sound_timer = chip8->V[x];
}

// 0xFX1E: ADD I, Vx - I += Vx
void inst_FX1E(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  chip8->I += chip8->V[x];
}

// 0xFX29: LD F, Vx - I = endereço do sprite do caractere em Vx
// Cada caractere ocupa 5 bytes, então o endereço é 0x050 + (Vx * 5)
void inst_FX29(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t digit = chip8->V[x] & 0x0F; // Apenas os 4 bits menos significativos
  chip8->I = 0x050 + (digit * 5);
}

// 0xFX33: LD B, Vx - Armazenar representação BCD (Binary Coded Decimal) de Vx
// Armazena centenas em I, dezenas em I+1, unidades em I+2
void inst_FX33(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t value = chip8->V[x];
  chip8->memory[chip8->I] = value / 100;        // Centenas
  chip8->memory[chip8->I + 1] = (value / 10) % 10; // Dezenas
  chip8->memory[chip8->I + 2] = value % 10;     // Unidades
}

// 0xFX55: LD [I], Vx - Salvar V0 até Vx na memória começando em I
void inst_FX55(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  for (int i = 0; i <= x; i++) {
    chip8->memory[chip8->I + i] = chip8->V[i];
  }
}

// 0xFX65: LD Vx, [I] - Carregar V0 até Vx da memória começando em I
void inst_FX65(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  for (int i = 0; i <= x; i++) {
    chip8->V[i] = chip8->memory[chip8->I + i];
  }
}

// Instruções de Teclado

// 0xEX9E: SKP Vx - Pular próxima instrução se tecla com valor Vx estiver pressionada
void inst_EX9E(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t key = chip8->V[x] & 0x0F; // Apenas os 4 bits menos significativos (0-15)

  if (chip8->keypad[key]) {
    chip8->pc += 2; // Pula a próxima instrução
  }
}

// 0xEXA1: SKNP Vx - Pular próxima instrução se tecla com valor Vx NÃO estiver pressionada
void inst_EXA1(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t key = chip8->V[x] & 0x0F; // Apenas os 4 bits menos significativos (0-15)

  if (!chip8->keypad[key]) {
    chip8->pc += 2; // Pula a próxima instrução
  }
}

// Instrução de Desenho

// 0xDXYN: DRW Vx, Vy, nibble - Desenhar sprite de N bytes na posição (Vx, Vy)
// Lê N bytes da memória começando em I
// Cada bit 1 inverte o pixel correspondente (XOR)
// VF é setado para 1 se algum pixel foi apagado (mudou de 1 para 0)
void inst_DXYN(Chip8* chip8, uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint8_t n = opcode & 0x000F;

  uint8_t vx = chip8->V[x] % 64; // Wrap-around horizontal
  uint8_t vy = chip8->V[y] % 32; // Wrap-around vertical

  chip8->V[0xF] = 0; // Flag de colisão começa em 0

  // Para cada linha do sprite (N bytes)
  for (int row = 0; row < n; row++) {
    uint8_t sprite_byte = chip8->memory[chip8->I + row];
    uint8_t py = (vy + row) % 32; // Posição Y na tela (com wrap-around)

    // Para cada bit do byte (8 pixels horizontais)
    for (int col = 0; col < 8; col++) {
      uint8_t px = (vx + col) % 64; // Posição X na tela (com wrap-around)
      int pixel_index = py * 64 + px;

      // Verifica se o bit correspondente está setado no sprite
      if (sprite_byte & (0x80 >> col)) {
        // Se o pixel na tela já estava ligado, há colisão
        if (chip8->display[pixel_index] == 1) {
          chip8->V[0xF] = 1;
        }
        // Inverte o pixel (XOR)
        chip8->display[pixel_index] ^= 1;
      }
    }
  }
}
