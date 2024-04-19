CC = gcc
CFLAGS =  -Wall -g
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

mallocTest: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) mallocTest
