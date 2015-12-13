#include "defines.h"
#include "basicMovement.h"

int main()
{
	connect_to_robot();
	initialize_robot();
	turnByAngleDegree(360.0);
	return 0;
}