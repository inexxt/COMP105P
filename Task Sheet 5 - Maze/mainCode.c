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

void printQueue(Queue* q)
{
	printf("PRINTING QUEUE\n");
	int i = 0;
	Queue* current = q;
	while (current != NULL)
	{
		printf("Q[%d] : x %d y %d \n", i, current->sxy.x, current->sxy.y);
        current = current->next;
    }
}


int main() 
{
	connect_to_robot();
	initialize_robot();
	set_ir_angle(LEFT, -45);
	set_ir_angle(RIGHT, 45);  
	
	set_motors(-10,-10);  // SUPER TEMP TODO (for some reason x,y update there, lol);
	sleep(2); // tmep
	reset_motor_encoders(); // temp
	set_origin();
	xPos = 0;
	yPos = 0;
	bearing = 0;
// 	goToSector(test);

	set_motors(0,0);

	yPos = -600;
	//////////////////////////// phase1
	Queue* currentPath = NULL;
	XY current;
	
	XY first = {.x = 0, .y = 0};
	maze[0][0].visited = 1;

	pushBack(&currentPath, first);
	printf("beginning\n");
	
	while(current.y != -1 && current.x != -1)
	{
		printQueue(currentPath);
		current = nextSector(currentPath);
		printfSector(current);
		goToXY(current);
		updateSector(current, currentPath);
	}

// 	endPhase1();
	/////////////////////////// 
	
	return 0;
}