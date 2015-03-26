CC=gcc
CFLAGS=-c -Wall
SOURCES=task40code.c phase1.c phase1.map.c phase1.move.c recordPosition.c picomms.c basicMovement.c phase2.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=task40

all:    $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):  $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ -lm

.c.o:
	$(CC) $(CFLAGS) $< -o $@
