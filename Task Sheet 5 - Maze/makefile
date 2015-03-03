CC=gcc
CFLAGS=-g -Wall
all:	mazeTask
mazeTask:	mainCode.o recordPosition.o basicMovement.o picomms.o
	$(CC) $(CFLAGS) -o mazeTask mainCode.o recordPosition.o basicMovement.o picomms.o -lm
mainCode.o:	mainCode.c -lm
	$(CC) $(CFLAGS) -c mainCode.c
recordPosition.o:	recordPosition.c recordPosition.h
	$(CC) $(CFLAGS) -c recordPosition.c -lm
basicMovement.o:	basicMovement.c basicMovement.h
	$(CC) $(CFLAGS) -c basicMovement.c
picomms.o:	picomms.c picomms.h
	$(CC) $(CFLAGS) -c picomms.c
