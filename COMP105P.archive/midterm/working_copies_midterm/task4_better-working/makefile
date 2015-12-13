CC= gcc
CFLAGS1=-Wall -c
CFLAGS2=-Wall
all: task2.1

task2.1: picomms.o static_functions.o task2.1.o
	$(CC) $(CFLAGS2) picomms.o static_functions.o task2.1.o -o task2.1 -lm

picomms.o: picomms.c
	$(CC) $(CFLAGS1) picomms.c

static_functions.o: static_functions.c
	$(CC) $(CFLAGS1) static_functions.c

task2.1.o: task2.1.c
	$(CC) $(CFLAGS1) task2.1.c
