#include "defines.h"
#include "phase1.map.h"
#include "phase1.move.h"
//#include "phase2.h"

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
	XY first = {.x = 0, .y = 0};
	XY current;
	currentPath = malloc(sizeof(Queue));
	currentPath->
	
	while(current.y != -1 && current.x != -1)
	{
		current = nextSector();
		goToXY(current);
		updateSector(current);
	}

// 	endPhase1();
	/////////////////////////// 
	
	return 0;
}