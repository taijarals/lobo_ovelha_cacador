CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Iinclude

LIBS = $(shell pkg-config --libs raylib)
CFLAGS += $(shell pkg-config --cflags raylib)

SRC := $(shell find src -name "*.c")
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

TARGET = bin/game

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(OBJ) -o $@ $(LIBS)

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
	rm -rf bin

run: all
	./$(TARGET)

.PHONY: all clean run