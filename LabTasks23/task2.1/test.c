#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include "static_functions.h"
#include <math.h>
#define SLEEPTIME 1000
#define BS 10
#define ST 10000

int main()
{
	int i=0;
	connect_to_robot();
	initialize_robot();
	set_ir_angle(RIGHT, 0);
	set_ir_angle(LEFT, -15);
	sleep(1);
	while(1)
	{
		for(i = 0; i>=-90; i--)
		{
			set_ir_angle(RIGHT, i);
			printf("Angle %d\tRight Distance %d Left Distance%d\n", i, rightFsensor(), leftFsensor());
			usleep(ST);
		}
		for(i = -90; i<=0; i++)
		{
			set_ir_angle(RIGHT, i);
			printf("Angle %d\tRight Distance %d Left Distance%d\n", i, rightFsensor(), leftFsensor());
			usleep(ST);
		}
		
	}
}
