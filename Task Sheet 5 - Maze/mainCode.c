#include "defines.h"
#include "phase1.map.h"
#include "phase1.move.h"
// #include "phase2.h"

int main() 
{
	connect_to_robot();
	initialize_robot();

	set_motors(-10,-10);  // SUPER TEMP TODO (for some reason x,y update there, lol);
	sleep(2); // tmep
	reset_motor_encoders(); // temp
	set_origin();
	xPos = 0;
	yPos = 0;
	bearing = 0;

	set_ir_angle(LEFT, 0);
	set_ir_angle(RIGHT, -75);   
	XY test = {.x = 0, .y = 2} ;

	goToSector(test);

	set_motors(0,0);

	struct XY act = {.x = 1, .y = 1} ;

	while(act.y != -1)
	{
		goToSector(act);
		updateSector(act);
		act = nextSector();
	}
	//go to sector(0,-1);
	prepareToPhase2();
	return 0;
}