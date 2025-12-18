#ifndef CHIP8_H // Previne a inclusão múltipla do arquivo
#define CHIP8_H // Define o macro CHIP8_H para evitar inclusões múltiplas

#include <stdint.h>

typedef struct { // Estrutura para o Chip8
    uint8_t  memory[4096];      // 4KB (4096 bytes) de memória
    uint8_t  V[16];             // V0–VF
    uint16_t I;                 // registrador de índice
    uint16_t pc;                // registrador de programa

    uint16_t stack[16];         // pilha de 16 posições
    uint8_t  sp;                // ponteiro de pilha

    uint8_t  delay_timer;       // timer de delay
    uint8_t  sound_timer;       // timer de som

    uint8_t  display[64 * 32];  // 0 ou 1 (64x32 pixels)
    uint8_t  keypad[16];        // teclado de 16 teclas

} Chip8;

void chip8_init(Chip8 *c); // Inicializa o Chip8
int chip8_load_rom(Chip8 *c, const char *path); // Carrega o ROM do Chip8 (retorna 0 em sucesso, -1 se erro)
void chip8_cycle(Chip8 *c); // Executa um ciclo do Chip8

#endif
