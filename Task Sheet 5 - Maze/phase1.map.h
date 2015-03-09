#include "phase1.h"

struct Sector
{
	int northWall;
	int southWall;
	int westWall;
	int eastWall;
	int visited;
};



extern struct Sector maze[4][4];



//void initialiseMaze();
void updateSector(int northWall, int southWall, int westWall, int eastWall);