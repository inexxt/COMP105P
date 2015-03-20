#include "defines.h"
#include "phase1.map.h"
#include "phase1.move.h"
//#include "phase2.h"

void debug()
{
	printCurrentSector();
}

void printfSector(XY s)
{
	printf("sector x %d y%d", s.x, s.y);
}

// print Queue poszedl do phase1.map.c by kompilator sie nie burzyl


int main() 
{
	connect_to_robot();
	initialize_robot();
	

// 	printf("beginning\n");
	//////////////////////////// phase1
	Queue* currentPath = NULL;
	XY current;
	
	centerStartingPosition();
	XY first = {.x = 0, .y = 0};
	XY second = {.x = 0, .y = 1};
	XY third = {.x = 1,. y = 1};
// 	goToXY(first);
// 	goToXY(second);
// 	goToXY(third);

	maze[0][0].visited = 2;

	pushFront(&currentPath, first);

	while(current.y != -1 && current.x != -1)
	{
		if(current.x == 0 && current.y == 0) set_origin();
		printf("BEING IN %d %d\n", current.x, current.y);
		printQueue(currentPath);
		current = nextSector(&currentPath);
		printfSector(current);
		goToXY(current);
		updateSector(&currentPath);
	}

// 	endPhase1();
	/////////////////////////// 
	
	return 0;
}