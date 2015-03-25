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

	// XY second = {.x = 0, .y = 1};
	// XY third = {.x = 1, .y = 1};

	// goToXY(first);
	// set_motors(0,0);
	// goToXY(second);
	// 	set_motors(0,0);
	// goToXY(third);
	// 	set_motors(0,0);

	int setOrigin = 1; // so it wouldnt try to set origin again
	while(current.y != -1 && current.x != -1)
	{
// 		printf("BEING IN %d %d\n", current.x, current.y);
// 		printQueue(currentPath);
		current = nextSector(&currentPath);
// 		printfSector(current);
		printf("about to go to XY\n");
		goToXY(current);
		printf("about to updateSector\n");
		updateSector(&currentPath);
		printf("about to correctPosition\n");
		correctPosition(current);
		if((current.x == 0 && current.y == 0) && (setOrigin))
		{
			set_origin();
			setOrigin = 0;
		}
	}
		
	endPhase1();
	
	Queue* a = calculateOptimalPath();
	while(!isEmpty(a))
	{
		goToXY(popFront(&a));
	}
	set_motors(0,0);
	
	double frontLeft, frontRight;
	getFrontIR(&frontLeft, &frontRight);
	if(((frontLeft+frontRight)/2) < (SECTOR_WIDTH/2))
	{
		set_motors(2,2);
		sleep(2);
		set_motors(0,0);
	}
	printf("FINISH\n");
	///////////////////////// 
	
	return 0;
}