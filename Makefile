CC = gcc
CFLAGS = -Wall -g
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
LIBNAME = libhmm

dynamic: $(LIBNAME).so

static: $(LIBNAME).a

$(LIBNAME).so: $(OBJ)
	$(CC) -shared -o $@ $^

$(LIBNAME).a: $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

clean:
	rm -f $(OBJ) $(LIBNAME).so $(LIBNAME).a
