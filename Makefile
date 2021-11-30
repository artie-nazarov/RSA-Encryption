CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic $(shell pkg-config --cflags gmp)
LDFLAGS = $(shell pkg-config --libs gmp)
OBJ = rsa.o randstate.o numtheory.o

all: keygen encrypt decrypt

keygen: keygen.o $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)


encrypt: encrypt.o $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)


decrypt: decrypt.o $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file *.c *.h

clean:
	rm -f keygen encrypt decrypt *.o

cleankeys:
	rm -f *. {pub,priv}
