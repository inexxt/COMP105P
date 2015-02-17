CC= gcc
CFLAGS1=-Wall -c
CFLAGS2=-Wall
all: task

task: picomms.o static_functions.o task.o
	$(CC) $(CFLAGS2) picomms.o static_functions.o task.o -o task -lm

picomms.o: picomms.c
	$(CC) $(CFLAGS1) picomms.c

static_functions.o: static_functions.c
	$(CC) $(CFLAGS1) static_functions.c

task.o: task.c
	$(CC) $(CFLAGS1) task.c
