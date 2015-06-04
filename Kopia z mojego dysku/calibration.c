#include "defines.h"
#include "basicMovement.h"

int main()
{
	connect_to_robot();
	initialize_robot();
	turnByAngleDegree(1080.0);
	return 0;
}