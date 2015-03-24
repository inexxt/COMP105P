#include "phase1.h"

void updateSector(Queue** currentPath);
XY nextSector(Queue** currentPath);
void pushFront(Queue** q, XY sxy);
double convertToDegrees(double radians);
void printQueue(Queue* q);