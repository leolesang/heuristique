CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
SRC = src/main.c src/utils.c src/io.c src/algo.c src/test.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
	EXEC = heuristique.exe
	CFLAGS += -D _CRT_SECURE_NO_WARNINGS
    RM = del /Q
	OBJ_DELETE = src\*.o
else
    RM = rm -rf
	EXEC = heuristique
	OBJ_DELETE = src/*.o
endif

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DELETE), $(EXEC)

.PHONY: all clean