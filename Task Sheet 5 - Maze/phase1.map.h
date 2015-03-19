#include "phase1.h"

//void initialiseMaze();
void updateSector();
XY nextSector();
// struct queue currentPath;
void pushBack(Queue** q, XY sxy); //reference in mainCode.c: pushBack(&currentPath, first);