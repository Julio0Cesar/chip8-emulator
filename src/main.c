#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "chip8.h"

// Constantes
#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SCALE 10
#define CYCLES_PER_FRAME 10  // Número de ciclos CHIP-8 por frame
#define TIMER_HZ 60          // Frequência dos temporizadores (60 Hz)
#define TARGET_FPS 60        // FPS alvo

// Mapeamento de teclas SDL2 para teclado do CHIP-8
// Layout do CHIP-8 original:
// 1 2 3 C
// 4 5 6 D
// 7 8 9 E
// A 0 B F
// Mapeado para teclado QWERTY:
// 1 2 3 4
// Q W E R
// A S D F
// Z X C V
static const SDL_Scancode keymap[16] = {
  SDL_SCANCODE_X,  // 0
  SDL_SCANCODE_1,  // 1
  SDL_SCANCODE_2,  // 2
  SDL_SCANCODE_3,  // 3
  SDL_SCANCODE_Q,  // 4
  SDL_SCANCODE_W,  // 5
  SDL_SCANCODE_E,  // 6
  SDL_SCANCODE_A,  // 7
  SDL_SCANCODE_S,  // 8
  SDL_SCANCODE_D,  // 9
  SDL_SCANCODE_Z,  // A
  SDL_SCANCODE_C,  // B
  SDL_SCANCODE_4,  // C
  SDL_SCANCODE_R,  // D
  SDL_SCANCODE_F,  // E
  SDL_SCANCODE_V   // F
};

int main(int argc, char* argv[]) {
  // Verifica argumentos
  if (argc != 2) {
    printf("Uso: %s <arquivo_rom>\n", argv[0]);
    return 1;
  }

  // Inicializa SDL2
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
    return 1;
  }

  // Cria janela (64x32 pixels escalados)
  SDL_Window* window = SDL_CreateWindow(
    "CHIP-8 Emulator",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    CHIP8_WIDTH * SCALE,
    CHIP8_HEIGHT * SCALE,
    SDL_WINDOW_SHOWN
  );
  if (!window) {
    printf("Erro ao criar janela: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Cria renderizador
  SDL_Renderer* renderer = SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_ACCELERATED
  );
  if (!renderer) {
    printf("Erro ao criar renderizador: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Cria textura para o display
  // Usa formato nativo do renderer para melhor performance
  SDL_Texture* texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING,
    CHIP8_WIDTH,
    CHIP8_HEIGHT
  );
  if (!texture) {
    printf("Erro ao criar textura: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Inicializa CHIP-8
  Chip8 chip8;
  chip8_init(&chip8);

  // Carrega ROM
  int rom_result = chip8_load_rom(&chip8, argv[1]);
  if (rom_result != 0) {
    switch (rom_result) {
      case -1:
        printf("Erro: Arquivo não encontrado: %s\n", argv[1]);
        break;
      case -2:
        printf("Erro: ROM muito grande (máximo 3584 bytes)\n");
        break;
      case -3:
        printf("Erro: Falha ao ler o arquivo: %s\n", argv[1]);
        break;
      default:
        printf("Erro desconhecido ao carregar ROM: %s\n", argv[1]);
        break;
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Variáveis para controle de loop e temporizadores
  int running = 1;
  uint32_t last_timer_tick = SDL_GetTicks();
  uint32_t last_frame_time = SDL_GetTicks();
  const uint32_t frame_delay = 1000 / TARGET_FPS; // Delay em milissegundos para 60 FPS

  // Loop principal
  while (running) {
    // Processa eventos SDL
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }
    }

    // Atualiza keypad com estado atual do teclado
    // Usa SDL_GetKeyboardState para obter estado atual de todas as teclas
    SDL_PumpEvents();
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

    // Atualiza keypad: mantém estado enquanto tecla estiver pressionada
    // Isso permite que instruções EX9E e EXA1 funcionem corretamente
    for (int i = 0; i < 16; i++) {
      chip8.keypad[i] = keyboard_state[keymap[i]] ? 1 : 0;
    }

    // Executa múltiplos ciclos do CHIP-8 por frame
    for (int i = 0; i < CYCLES_PER_FRAME; i++) {
      chip8_cycle(&chip8);
    }

    // Decrementa temporizadores a 60 Hz (independentemente da velocidade de execução)
    uint32_t now = SDL_GetTicks();
    if (now - last_timer_tick >= (1000 / TIMER_HZ)) {
      if (chip8.delay_timer > 0) {
        chip8.delay_timer--;
      }
      if (chip8.sound_timer > 0) {
        chip8.sound_timer--;
        // TODO: Emitir beep aqui (usando SDL_mixer ou similar)
      }
      last_timer_tick = now;
    }

    // Renderiza display
    // Converte array display[] (valores 0/1) em pixels RGBA8888
    // Usa SDL_MapRGBA para garantir ordem correta de bytes
    void* pixels_ptr;
    int pitch;
    if (SDL_LockTexture(texture, NULL, &pixels_ptr, &pitch) == 0) {
      SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
      uint32_t white = SDL_MapRGBA(format, 255, 255, 255, 255);
      uint32_t black = SDL_MapRGBA(format, 0, 0, 0, 255);

      // Converte diretamente para a textura, linha por linha
      // IMPORTANTE: pitch pode ser maior que width * sizeof(uint32_t) devido ao alinhamento
      uint8_t* dst = (uint8_t*)pixels_ptr;
      for (int y = 0; y < CHIP8_HEIGHT; y++) {
        uint32_t* row = (uint32_t*)(dst + y * pitch);
        for (int x = 0; x < CHIP8_WIDTH; x++) {
          // Calcula índice no array display: y * width + x
          int display_index = y * CHIP8_WIDTH + x;
          row[x] = chip8.display[display_index] ? white : black;
        }
      }

      SDL_FreeFormat(format);
      SDL_UnlockTexture(texture);
    }

    // Desenha na janela
    SDL_RenderClear(renderer);
    // Define retângulo de destino para escalar a textura 64x32 para a janela escalada
    SDL_Rect dest_rect = {0, 0, CHIP8_WIDTH * SCALE, CHIP8_HEIGHT * SCALE};
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
    SDL_RenderPresent(renderer);

    // Controle de FPS - mantém 60 FPS
    uint32_t frame_time = SDL_GetTicks() - last_frame_time;
    if (frame_time < frame_delay) {
      SDL_Delay(frame_delay - frame_time);
    }
    last_frame_time = SDL_GetTicks();
  }

  // Limpeza
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
