# CHIP-8 Emulator

Um emulador CHIP-8 completo e funcional escrito em C, utilizando SDL2 para renderização gráfica e entrada de teclado.

![CHIP-8](https://img.shields.io/badge/CHIP--8-Emulator-blue)
![C](https://img.shields.io/badge/Language-C-orange)
![SDL2](https://img.shields.io/badge/Library-SDL2-green)

![Captura](games/Space%20Invaders.png)

## 📋 Sobre

CHIP-8 é uma linguagem de programação interpretada, desenvolvida originalmente para computadores como o COSMAC VIP e o Telmac 1800. Este emulador implementa todas as 35 instruções do conjunto de instruções CHIP-8, permitindo executar jogos e programas clássicos.

## ✨ Características

- ✅ Implementação completa de todas as 35 instruções CHIP-8
- ✅ Display de 64x32 pixels com renderização via SDL2
- ✅ Suporte completo ao teclado hexadecimal CHIP-8 (16 teclas)
- ✅ Temporizadores de delay e som
- ✅ Sistema de sprites com detecção de colisão
- ✅ Carregamento de ROMs de até 3584 bytes
- ✅ Execução a 60 FPS com 10 ciclos por frame

## 🛠️ Requisitos

- **Compilador C**: GCC ou Clang
- **SDL2**: Biblioteca de desenvolvimento SDL2
- **Sistema Operacional**: Linux, macOS ou Windows (com WSL2)

### Instalação das Dependências

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev
```

#### Fedora/RHEL
```bash
sudo dnf install gcc make SDL2-devel
```

#### macOS (com Homebrew)
```bash
brew install sdl2
```

## 📦 Compilação

1. Clone o repositório:
```bash
git clone <seu-repositorio>
cd C
```

2. Compile o projeto:
```bash
make
```

Ou compile manualmente:
```bash
gcc -Wall -Wextra -std=c11 -O2 -Iinclude src/*.c -o chip8 -lSDL2
```

3. Para limpar arquivos de compilação:
```bash
make clean
```

## 🎮 Uso

Execute o emulador com um arquivo ROM:

```bash
./chip8 <caminho_para_rom>
```

Exemplo:
```bash
./chip8 games/pong.ch8
```

## ⌨️ Mapeamento de Teclas

O emulador mapeia o teclado hexadecimal CHIP-8 para o layout QWERTY:

### Layout CHIP-8 Original
```
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

### Mapeamento para Teclado QWERTY
```
1 2 3 4
Q W E R
A S D F
Z X C V
```

Onde:
- **1, 2, 3, 4** → Teclas numéricas 1, 2, 3, 4
- **Q, W, E, R** → Teclas Q, W, E, R
- **A, S, D, F** → Teclas A, S, D, F
- **Z, X, C, V** → Teclas Z, X, C, V

## 📁 Estrutura do Projeto

```
C/
├── src/              # Código-fonte (.c)
│   ├── main.c        # Loop principal e renderização SDL2
│   ├── chip8.c       # Inicialização e ciclo do emulador
│   └── instructions.c # Implementação das instruções CHIP-8
├── include/          # Cabeçalhos (.h)
│   ├── chip8.h       # Estrutura e funções principais
│   └── instructions.h # Declarações das instruções
├── games/            # ROMs de jogos CHIP-8
├── build/            # Arquivos objeto (gerado na compilação)
├── Makefile          # Arquivo de build
└── README.md         # Este arquivo
```

## 🔧 Instruções Implementadas

### Instruções de Sistema
- `00E0` - CLS: Limpar tela
- `00EE` - RET: Retornar de sub-rotina

### Instruções de Salto
- `1NNN` - JP addr: Pular para endereço
- `2NNN` - CALL addr: Chamar sub-rotina
- `BNNN` - JP V0, addr: Pular para V0 + addr

### Instruções Condicionais
- `3XKK` - SE Vx, byte: Pular se Vx == KK
- `4XKK` - SNE Vx, byte: Pular se Vx != KK
- `5XY0` - SE Vx, Vy: Pular se Vx == Vy
- `9XY0` - SNE Vx, Vy: Pular se Vx != Vy

### Instruções de Atribuição e Aritmética
- `6XKK` - LD Vx, byte: Vx = KK
- `7XKK` - ADD Vx, byte: Vx += KK
- `8XY0` - LD Vx, Vy: Vx = Vy
- `8XY1` - OR Vx, Vy: Vx |= Vy
- `8XY2` - AND Vx, Vy: Vx &= Vy
- `8XY3` - XOR Vx, Vy: Vx ^= Vy
- `8XY4` - ADD Vx, Vy: Vx += Vy (com carry)
- `8XY5` - SUB Vx, Vy: Vx -= Vy (com borrow)
- `8XY6` - SHR Vx: Shift right
- `8XY7` - SUBN Vx, Vy: Vx = Vy - Vx
- `8XYE` - SHL Vx: Shift left

### Instruções de Índice e Memória
- `ANNN` - LD I, addr: I = NNN
- `FX07` - LD Vx, DT: Vx = delay_timer
- `FX15` - LD DT, Vx: delay_timer = Vx
- `FX18` - LD ST, Vx: sound_timer = Vx
- `FX1E` - ADD I, Vx: I += Vx
- `FX29` - LD F, Vx: I = endereço do sprite
- `FX33` - LD B, Vx: Armazenar BCD
- `FX55` - LD [I], Vx: Salvar registradores
- `FX65` - LD Vx, [I]: Carregar registradores

### Instruções de Teclado
- `EX9E` - SKP Vx: Pular se tecla Vx pressionada
- `EXA1` - SKNP Vx: Pular se tecla Vx não pressionada
- `FX0A` - LD Vx, K: Aguardar tecla e armazenar em Vx

### Instrução de Desenho
- `DXYN` - DRW Vx, Vy, nibble: Desenhar sprite

## 🐛 Últimas Correções Implementadas

- ✅ Correção do cálculo de índice de pixel (overflow em `uint8_t`)
- ✅ Implementação das instruções `EX9E` e `EXA1` (faltantes)
- ✅ Correção da detecção de teclas para evitar loops automáticos
- ✅ Melhoria na renderização do display

## 📝 Notas

- O emulador executa a 60 FPS com 10 ciclos CHIP-8 por frame
- Os temporizadores são decrementados a 60 Hz
- O display é renderizado usando SDL2 com escala de 10x
- ROMs devem ter no máximo 3584 bytes (espaço disponível a partir de 0x200)

## 🤝 Contribuindo

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou pull requests.

## 📄 Licença

Este projeto é de código aberto. Sinta-se livre para usar e modificar conforme necessário.

## 🙏 Agradecimentos

- Especificação CHIP-8: [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- SDL2: [Simple DirectMedia Layer](https://www.libsdl.org/)
- Guia Téorico: [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)

---

Desenvolvido com ❤️ em C
