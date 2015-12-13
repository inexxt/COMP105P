CC= gcc
CFLAGS= -Wall

all: task2.1

task2.1: picomms.o static_functions.o task2.1.o
	$(CC) $(CFLAGS) picomms.o static_functions.o task2.1.o -o task2.1

picomms.o: picomms.c
	$(CC) $(CFLAGS) picomms.c

static_functions.o: static_functions.c
	$(CC) $(CFLAGS) static_functions.c

task2.1.o: task2.1.c
	$(CC) $(CFLAGS) task2.1.c