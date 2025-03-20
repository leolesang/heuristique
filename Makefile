CC = gcc
LDFLAGS = -lm
SRC = src/main.c src/utils.c src/io.c src/algo.c src/test.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
    EXEC = heuristique.exe
	CFLAGS = -Wall -Wextra -std=c99 -O2 -D _CRT_SECURE_NO_WARNINGS
    RM = del /Q
    OBJ_DELETE = src\*.o
else
	CFLAGS = -Wall -Wextra -std=c99 -O2 -m64 -fstack-protector
    RM = rm -rf
    EXEC = heuristique
    OBJ_DELETE = src/*.o
endif

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DELETE) $(EXEC)

.PHONY: all clean
