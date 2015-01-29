CC= gcc
CFLAGS=-c -lm

all: task2.0

task2.0: picomms.o static_functions.o task2.0.o
	$(CC) picomms.o static_functions.o task2.0.o -o task2.0

picomms.o: picomms.c
	$(CC) $(CFLAGS) picomms.c

static_functions.o: static_functions.c
	$(CC) $(CFLAGS) static_functions.c

task2.0.o: task2.0.c
	$(CC) $(CFLAGS) task2.0.c
