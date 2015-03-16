#include "phase1.move.h"
#include "phase1.map.h"

double xPos, yPos, bearing;

void correctPosition()
{
  // TODO


}

void goToXY(XY destination)
{
	double xDifference;
	double yDifference;
	double requiredAngleChange;
	double remainingDistance = 10000.00;

	double xCoordinate = destination.x * SECTOR_WIDTH; //TODO
	double yCoordinate = destination.y * SECTOR_WIDTH; //TODO
	printf("\nCoordinates: %f\t%f", xCoordinate, yCoordinate); // debug
	printf("\n%f %f %f", xPos, yPos, bearing); // debug
	while (remainingDistance > 10) // value to change
	{
		printf("\nCurrent X: %f\t current Y: %f \t current bearing: %f \n\n",xPos, yPos, bearing); // debug
		updateRobotPosition(); 
		xDifference = xCoordinate - xPos;
		yDifference = yCoordinate - yPos;
		requiredAngleChange = atan2(xDifference, yDifference) - bearing;
		remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);

		if(fabs(requiredAngleChange) > 1.2) 
		{
			while(fabs(requiredAngleChange) > 0.10)
			{
				updateRobotPosition(); 
				requiredAngleChange = atan2(xDifference, yDifference) - bearing;
				set_motors(requiredAngleChange*14.0, -requiredAngleChange*14.0);
			}
		}
		else
		{
			updateRobotPosition(); 
			set_motors((MEDIUM_SPEED + MEDIUM_SPEED * requiredAngleChange*1.2),(MEDIUM_SPEED - MEDIUM_SPEED * requiredAngleChange*1.2));

			// TODO: add sensor correction
		}
	}
	set_motors(0,0);
	correctPosition();
}