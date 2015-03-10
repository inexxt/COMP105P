#include "phase1.h"

double xPos, yPos;

struct XY getCurrentSector()
{
	int xSquare = round(xPos/SECTOR_WIDTH);
	int ySquare = round(yPos/SECTOR_WIDTH);

	struct XY currentPosition = {.x = xSquare, .y = ySquare};

	return currentPosition;
}