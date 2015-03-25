#include "defines.h"
#include "phase1.map.h"
#include "phase1.move.h"
#include "phase2.h"

void debug()
{
	printCurrentSector();
}

void printfSector(XY s)
{
	printf("sector x %d y%d", s.x, s.y);
}

void printfMaze()
{
	int i,j;
	for(i = 0; i<4; i++)
		for(j = 0; j<4; j++)
		{
			printf("maze[%d][%d] = (Sector){%d, %d, %d, %d, %d};\n", i, j, maze[i][j].northWall, maze[i][j].southWall, maze[i][j].westWall, maze[i][j].eastWall, maze[i][j].visited);
		}
}

// print Queue poszedl do phase1.map.c by kompilator sie nie burzyl

// double xPos, yPos, bearing;
int main() 
{
	connect_to_robot();
	initialize_robot();
	printf("beginning\n");
	////////////////////////// phase1
	Queue* currentPath = NULL;
	XY current;

	XY first = {.x = 0, .y = 0};
	maze[0][0].visited = 2;
    pushFront(&currentPath, first);

	centerStartingPosition();
	int setOrigin = 1; // so it wouldnt try to set origin again
	while(current.y != -1 && current.x != -1)
	{
		current = nextSector(&currentPath);
		goToXY(current);
		updateSector(&currentPath);
		correctPosition(current);
		if((current.x == 0 && current.y == 0) && (setOrigin))
		{
			set_origin();
			setOrigin = 0;
		}
	}
		
	printfMaze();
	// endPhase1();
	
	// Queue* a = calculateOptimalPath();
	// while(!isEmpty(a))
	// {
	// 	goToXY(popFront(&a));
	// }
	// set_motors(0,0);
	
	// double frontLeft, frontRight;
	// getFrontIR(&frontLeft, &frontRight);
	// if(((frontLeft+frontRight)/2) < (SECTOR_WIDTH/2))
	// {
	// 	set_motors(2,2);
	// 	sleep(2);
	// 	set_motors(0,0);
	// }
	printf("FINISH\n");
	///////////////////////// 
	
	return 0;
}