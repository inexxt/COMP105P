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

XY getCurrentSector();
Sector maze[4][4];

#endif