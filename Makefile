CC = gcc
CFLAGS = -Wall -g

all: Source

Source: Source.c
	$(CC) $(CFLAGS) Source.c -o Source

clean:
	del Source.exe
