CC = gcc
CFLAGS= -Wall -Wextra
DEPS =
OBJ = Harkonen.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Harkonen: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)