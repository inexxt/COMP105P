#ifndef PHASE1_H
#define PHASE1_H

#include "defines.h"
typedef struct
{
	int x;
	int y;
} XY;

// visited explanation
// visited 0: exploring - robot is yet to enter the sector, wasn't there before
// visited 1: return: - robot has already visited the sector and is to return via it
// visited 2+: visited - robot shouldn't enter the sector now

typedef struct
{
    int northWall;
    int southWall;
    int westWall;
    int eastWall;
    int visited;
    double xCenter;
    double yCenter;
} Sector;

typedef struct Queue_t
{
    XY sxy;
    struct Queue_t * next;
} Queue;

XY getCurrentSector();
void printCurrentSector();

void getSideIR(double *left, double *right);
void getFrontIR(double *left, double *right);
double getUSValue();

extern Sector maze[MAZE_WIDTH][MAZE_HEIGHT];

#endif