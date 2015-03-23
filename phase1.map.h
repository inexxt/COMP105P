#include "phase1.h"

//void initialiseMaze();
void updateSector(Queue** currentPath);
XY nextSector(Queue** currentPath);
// struct queue currentPath;
void pushFront(Queue** q, XY sxy);
double convertToDegrees(double radians);
void printQueue(Queue* q);