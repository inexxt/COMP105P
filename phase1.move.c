#include "phase1.move.h"
#include "phase1.map.h"
#include "basicMovement.h"

#define SAFE_RANGE 25

double xPos, yPos, bearing;

void adjustAngle()
{
  int speedMultiplier = 2;
  int frontLeftIR, frontRightIR;
  set_ir_angle(LEFT, 20);
  set_ir_angle(RIGHT, -20);    
  get_front_ir_dists(&frontLeftIR, &frontRightIR);
  int sensorDifference;
  while((frontLeftIR > SAFE_RANGE) && (frontRightIR > SAFE_RANGE))
  {
    set_motors(5,5);
    get_front_ir_dists(&frontLeftIR, &frontRightIR);
  }
  set_motors(0,0);
  while(frontRightIR != frontLeftIR)
  {
    get_front_ir_dists(&frontLeftIR, &frontRightIR);
    sensorDifference = frontLeftIR - frontRightIR;
    if(sensorDifference > 5)
      sensorDifference = 5;
    if(sensorDifference < -5)
      sensorDifference = -5;
    set_motors((sensorDifference * speedMultiplier), (-sensorDifference * speedMultiplier));
  }
  set_motors(0,0);
}

void adjustCoordinates()
{
  int targetUS = SECTOR_WIDTH/2 - ROBOT_WIDTH/2 + 6;
  int ultraSound;
  ultraSound = get_us_dist();
  while(ultraSound != targetUS)
  {
    ultraSound = get_us_dist();
    int difference = ultraSound - targetUS;
    set_motors((difference*2),(difference*2));
  }
  set_motors(0,0);
}

void centerStartingPosition()
{
  int i;

  for(i = 0; i < 3; i++)
  {
    turnByAngleDegree(-90.00);

	usleep(400000);

    adjustAngle();
    usleep(400000);
    adjustCoordinates();
    usleep(400000);
  }
  turnByAngleDegree(-90.00);
  bearing = 0; // so that the bearing would be affected by this movement (so we would know it's slightly off ( important!))
  updateRobotPosition();
  xPos = 0;
  yPos = -60;
  updateRobotPosition();
}

void correctPosition()
{
  int ultraSound = get_us_dist();
  if(ultraSound < 35)
  {
    adjustAngle();
    usleep(400000);
    adjustCoordinates();
    //turnByAngleDegree(-90.00);
   // ultraSound = get_us_dist();
  }
}

void goToXY(XY destination)
{
  double xDifference;
  double yDifference;
  double requiredAngleChange;
  double remainingDistance = 10000.00;

  double xCoordinate = destination.x * SECTOR_WIDTH; //TODO
  double yCoordinate = destination.y * SECTOR_WIDTH; //TODO
  //printf("\nCoordinates: %f\t%f", xCoordinate,yCoordinate); // debug
//   printf("\n%f %f %f", xPos, yPos, bearing); // debug
  while ((fabs(remainingDistance)) > 5) // value to change
  {
	  set_point(xPos,yPos);
	  log_trail();
    printf("\nCurrent X: %f\t current Y: %f \t current bearing: %f \n\n",xPos,yPos,bearing); // debug
//     printf("remain %.2f\n", remainingDistance);
    updateRobotPosition(); 
  	xDifference = xCoordinate - xPos;
    yDifference = yCoordinate - yPos;
    requiredAngleChange = atan2(xDifference,yDifference) - bearing;
    while(requiredAngleChange > (M_PI))
    {
      requiredAngleChange -= (2 * M_PI);
    }
    while(requiredAngleChange < (-M_PI))
    {
      requiredAngleChange += (2 * M_PI);
    }
    // printf("angleCh: %f, bearing: %f\n",requiredAngleChange,bearing);
    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);

    if(fabs(requiredAngleChange) > 1.2) 
    {
    	while(fabs(requiredAngleChange) > 0.08)
    	{
				  set_point(xPos,yPos);
				  log_trail();

          updateRobotPosition(); 
          requiredAngleChange = atan2(xDifference,yDifference) - bearing;
          while(requiredAngleChange > (M_PI))
          {
            requiredAngleChange -= (2 * M_PI);
          }
          while(requiredAngleChange < (-M_PI))
          {
            requiredAngleChange += (2 * M_PI);
          }
          set_motors(requiredAngleChange*15.0,-requiredAngleChange*15.0);
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

