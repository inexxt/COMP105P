#ifndef PHASE1_H
#define PHASE1_H

#include "defines.h"
#include "recordPosition.h"

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
} Sector;

typedef struct Queue_t
{
	XY sxy;
	struct Queue_t * next;
} Queue;

XY getCurrentSector();
void printCurrentSector();

Sector maze[MAZE_WIDTH][MAZE_HEIGHT];


#endif