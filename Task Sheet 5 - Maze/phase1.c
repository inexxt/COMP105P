#include "phase1.h"

extern double xPos, yPos;
extern Sector maze[4][4];

XY getCurrentSector()
{
	int xSquare = round(xPos/SECTOR_WIDTH);
	int ySquare = round(yPos/SECTOR_WIDTH);

	return (XY){.x = xSquare, .y = ySquare};
}

void printCurrentSector()
{
	XY cs = getCurrentSector();
	printf("DEBUG CUR\tX %d Y %d\n", cs.x, cs.y);
}