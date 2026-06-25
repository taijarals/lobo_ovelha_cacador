CC = gcc
CFLAGS = -Iinclude
LDFLAGS = -Llib
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC = \
			src/main.c \
			src/entities/entities.c\
			src/background/background.c

default:
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o game
