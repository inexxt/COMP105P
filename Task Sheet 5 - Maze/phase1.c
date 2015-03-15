#include "phase1.h"

double xPos, yPos;

extern Sector maze[4][4];

XY getCurrentSector()
{
	int xSquare = round(xPos/SECTOR_WIDTH);
	int ySquare = round(yPos/SECTOR_WIDTH);

	XY currentPosition = {.x = xSquare, .y = ySquare};

	return currentPosition;
}