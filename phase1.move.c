#include "phase1.move.h"
#include "phase1.map.h"
#include "basicMovement.h"

#define SAFE_RANGE 25
#define MAX_IR_RANGE 32
#define LEFT_SIDE -1
#define RIGHT_SIDE 1

double xPos, yPos, bearing;
int targetIR = SECTOR_WIDTH/2 - ROBOT_WIDTH/2; //INICJALIZACJA TYLKO -> POTEM SIE WARTOSC ZMIENIA/POPRAWIA // wiem ze mozna jako lokanlna, ale wg. mnie w ten sposob jest bardziej czytelne i dostepne
// dotyczy side sensorow, front : side + 2;
int targetUS;

void adjustAngle()
{
  int speedMultiplier = 2;
  int frontLeftIR, frontRightIR;
  set_ir_angle(LEFT, 20);
  set_ir_angle(RIGHT, -20);    
  usleep(1000000);
  get_front_ir_dists(&frontLeftIR, &frontRightIR);
  int sensorDifference;
  while(((frontLeftIR > SAFE_RANGE) && (frontRightIR > SAFE_RANGE))  && ((frontLeftIR < MAX_IR_RANGE) && (frontRightIR < MAX_IR_RANGE)))
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
  int ultraSound;
  targetUS = targetIR + 2;
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
  int sideLeft, sideRight;
  get_side_ir_dists(&sideLeft, &sideRight);
  targetIR = ((double)(sideLeft + sideRight))/2;
  for(i = 0; i < 3; i++)
  {
    get_side_ir_dists(&sideLeft, &sideRight);
    if((sideLeft < MAX_IR_RANGE) && (sideRight < MAX_IR_RANGE))
      targetIR = ((double)(sideLeft + sideRight))/2;
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

void updateCoordinates(XY crs, int US, int value, int side)
{
//left - -1
  // right - 1
  int IROffSet = value - targetIR;
     printf(" bearing: %f, degree: %f\n",bearing, (convertToDegrees(bearing)));
   if(convertToDegrees(bearing) < 15 || convertToDegrees(bearing) > 345) //first case - it's heading north
   {
     xPos = crs.x * SECTOR_WIDTH - IROffSet * side; //
     yPos = crs.y * SECTOR_WIDTH; //
     printf("case north, side: %d \n", side);
   }
    
   if(convertToDegrees(bearing) < 105 && convertToDegrees(bearing) > 75) //second case - it's heading east
   {
     xPos = crs.x * SECTOR_WIDTH; //
     yPos = crs.y * SECTOR_WIDTH + IROffSet * side; //
        printf("case east, side: %d \n", side);
   }
    
   if(convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165) //third case - it's heading south
   {
     xPos = crs.x * SECTOR_WIDTH + IROffSet * side; //
     yPos = crs.y * SECTOR_WIDTH; //
        printf("case south, side: %d \n", side);
   }
    
   if(convertToDegrees(bearing) < 285 && convertToDegrees(bearing) > 255) //fourth case - it's heading west
   {
     xPos = crs.x * SECTOR_WIDTH; //
     yPos = crs.y * SECTOR_WIDTH - IROffSet * side;//
        printf("case west, side: %d \n", side);
   }
}

void correctPosition(XY crs)
{
  int wallCountNorth = maze[crs.x][crs.y].eastWall + maze[crs.x][crs.y].westWall; //sprawdza czy sa 2 przylegle sciany
  int wallCountSouth = maze[crs.x][crs.y].eastWall + maze[crs.x][crs.y].westWall;
  if(((maze[crs.x][crs.y].northWall) && (wallCountNorth > 0)) || ((maze[crs.x][crs.y].southWall) && (wallCountSouth > 0)))
  {
    adjustAngle();
    int ultraSound = get_us_dist();
    while(ultraSound != targetUS)
    {
      ultraSound = get_us_dist();
      int difference = ultraSound - targetUS;
      set_motors((difference*2),(difference*2));
    }
    set_motors(0,0);
    set_ir_angle(LEFT, -45);
    set_ir_angle(RIGHT, 45); 
    usleep(1000000);
    int sideLeft, sideRight;
    int frontLeft, frontRight;
    get_side_ir_dists(&sideLeft, &sideRight);
    get_front_ir_dists(&frontLeft, &frontRight);
    if((sideLeft < MAX_IR_RANGE) && (frontLeft < MAX_IR_RANGE))
    {
      int leftValue = ((double)(sideLeft + frontLeft))/2;
      updateCoordinates(crs, ultraSound, leftValue, LEFT_SIDE);
    }

    else if((sideRight < MAX_IR_RANGE) && (frontRight < MAX_IR_RANGE))
    {
      int rightValue = ((double)(sideRight + frontRight))/2;
      updateCoordinates(crs, ultraSound,rightValue,RIGHT_SIDE);
    }

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
  while ((fabs(remainingDistance)) > 3) // value to change
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
}

void endPhase1()
{
	turnByAngleDegree(-180.0);
	set_ir_angle(LEFT, 90);
	set_ir_angle(RIGHT, -90);
	sleep(2);
}
	
