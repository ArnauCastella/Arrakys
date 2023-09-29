CC = gcc
CFLAGS= -pthread -Wall -Wextra
DEPS = file.h input.h types.h commands.h
OBJ = Fremen.o file.o input.o commands.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Fremen: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)