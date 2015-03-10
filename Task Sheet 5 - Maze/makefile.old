CC=gcc
CFLAGS=-g -Wall
all:	mazeTask
mazeTask:	mainCode.o recordPosition.o picomms.o phase1.map.o phase1.move.o 
	$(CC) $(CFLAGS) -o mazeTask mainCode.o recordPosition.o picomms.o
mainCode.o:	mainCode.c
	$(CC) $(CFLAGS) -c mainCode.c



phase1.map.o:	phase1.map.c
	$(CC) $(CFLAGS) -c phase1.map.c
phase1.move.o:	phase1.move.c
	$(CC) $(CFLAGS) -c phase1.move.c




recordPosition.o:	recordPosition.c recordPosition.h
	$(CC) $(CFLAGS) -c recordPosition.c
picomms.o:	picomms.c picomms.h
	$(CC) $(CFLAGS) -c picomms.c




gcc -Wall -o mazeTask mainCode.c phase1.map.c phase1.move.c recordPosition.c picomms.c -lm

