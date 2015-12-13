CC=gcc
CFLAGS=-g -Wall
all:	task40
task40:	task40code.o recordPosition.o basicMovement.o picomms.o
	$(CC) $(CFLAGS) -o task40 task40code.o recordPosition.o basicMovement.o picomms.o -lm
task40code.o:	task40code.c -lm
	$(CC) $(CFLAGS) -c task40code.c
recordPosition.o:	recordPosition.c recordPosition.h
	$(CC) $(CFLAGS) -c recordPosition.c -lm
basicMovement.o:	basicMovement.c basicMovement.h
	$(CC) $(CFLAGS) -c basicMovement.c
picomms.o:	picomms.c picomms.h
	$(CC) $(CFLAGS) -c picomms.c
