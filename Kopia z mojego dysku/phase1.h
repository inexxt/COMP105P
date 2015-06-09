#ifndef PHASE1_H
#define PHASE1_H

#include "defines.h"
#include "recordPosition.h"

// TODO: dodac defines tego:
// visited 0: exploring - dopiero dojechac, jeszcze nie byl
// visited 1: return: ma przez to wracac
// visited 2: visited - juz w ogole nie wjezdzac
// visited 3: equivalent do 2


typedef struct
{
	int x;
	int y;
} XY;

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