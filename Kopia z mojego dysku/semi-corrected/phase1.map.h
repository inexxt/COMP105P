#ifndef PHASE1MAP
#define PHASE1MAP 1
#include "phase1.h"

void updateSector(Queue** currentPath);
XY nextSector(Queue** currentPath);
void pushFront(Queue** q, XY sxy);
XY popFront(Queue** q);
int isEmpty(Queue* q);
double convertToDegrees(double radians);
int printQueue(Queue* q);

#endif