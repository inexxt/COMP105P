#include "defines.h"
#include "basicMovement.h"





int main()
{
	connect_to_robot();
	initialize_robot();
	while(1)
	{
		printf("Choose action: \n");
		printf("1 go X forward\n");
		printf("2 turn by X degree\n");
		printf("3--- done.\n");
		int choice;
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
				goto escape;
				break;
		}
	}
	escape:
	return 0;
}