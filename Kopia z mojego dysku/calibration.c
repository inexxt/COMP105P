#include "defines.h"
#include "basicMovement.h"

int main()
{
	connect_to_robot();
	initialize_robot();
   int choice = 1;
	while(choice)
	{
		printf("Choose action: \n");
		printf("1 go X forward\n");
		printf("2 turn by X degree\n");
		printf("3--- done.\n");
		scanf("%d",&choice);
		float distance, angle;
		switch(choice)
		{
			case 1:
				printf("enter distance in cm you wish to move by: \n");
				scanf("%f",&distance);
				goStraight(distance);
				break;
			case 2:
				printf("enter the angle you wish to rotate by: \n");
				scanf("%f",&angle);
				turnByAngleDegree(angle);
				break;
			default:
				choice = 0;
				break;
		}
	}
	return 0;
}