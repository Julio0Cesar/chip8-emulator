# Makefile para o emulador CHIP-8

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude
LDFLAGS = -lSDL2

# Nome do executável
TARGET = chip8

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Arquivos fonte
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/chip8.c $(SRC_DIR)/instructions.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Criar diretório de build se não existir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Regra padrão
all: $(BUILD_DIR) $(TARGET)

# Link do executável
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compilação de arquivos objeto
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Recompilar tudo
rebuild: clean all

.PHONY: all clean rebuild
