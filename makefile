# Nome do compilador
CC = gcc

# Diretório dos arquivos de cabeçalho
INCLUDE_DIRS = -Iinclude -Icli-lib/include

# Diretório dos arquivos fonte
SRC_DIR = src
LIB_DIR = cli-lib/src

# Flags de compilação
CFLAGS = -Wall $(INCLUDE_DIRS)

# Arquivo de saída
OUTPUT = game

# Arquivos fonte
SOURCES = $(SRC_DIR)/main.c $(LIB_DIR)/keyboard.c $(LIB_DIR)/screen.c $(LIB_DIR)/timer.c

# Regras
all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)