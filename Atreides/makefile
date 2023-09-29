CC = gcc
CFLAGS= -pthread -Wall -Wextra
DEPS = connections.h types.h file.h linked_list.h
OBJ = Atreides.o file.o connections.o linked_list.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Atreides: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)