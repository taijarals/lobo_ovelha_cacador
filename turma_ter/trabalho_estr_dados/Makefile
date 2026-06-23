# Makefile - Simulador de Ecossistema
# uso: make / make run / make debug / make clean

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2
DBFLAGS = -Wall -Wextra -std=c99 -g -fsanitize=address,undefined
TARGET  = ecosistema
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SRCS = $(SRC_DIR)/main.c        \
       $(SRC_DIR)/entity.c      \
       $(SRC_DIR)/board.c       \
       $(SRC_DIR)/movement.c    \
       $(SRC_DIR)/interaction.c \
       $(SRC_DIR)/simulation.c  \
       $(SRC_DIR)/display.c

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo ">> Compilado: ./$(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

debug:
	$(CC) $(DBFLAGS) -I$(INC_DIR) $(SRCS) -o $(TARGET)_debug
	@echo ">> Debug: ./$(TARGET)_debug"

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TARGET)_debug
	@echo ">> Limpo."

run: all
	./$(TARGET)

.PHONY: all debug clean run
