CC=gcc
CFLAGS=-O2 -Wall -Wextra

all:
	$(CC) $(CFLAGS) src/*.c -o extrm

clean:
	rm -f extrm
