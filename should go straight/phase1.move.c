#include "phase1.move.h"
#include "phase1.map.h"
#include "basicMovement.h"

#define US_OFFSET 2 //difference between av. IR and US (simulator - 2, real robot ~ 6) - zeby latwiej bylo zmieniac
#define ADJUST_IR_ANGLE 25
#define MIN_IR_RANGE 12
#define MAX_IR_RANGE 36
#define SENSOR_THRESHOLD 0.9

double xPos, yPos, bearing;
int targetIR = SECTOR_WIDTH/2 - ROBOT_WIDTH/2; //INICJALIZACJA TYLKO -> POTEM SIE WARTOSC ZMIENIA/POPRAWIA // wiem ze mozna jako lokanlna, ale wg. mnie w ten sposob jest bardziej czytelne i dostepne
// dotyczy side sensorow, front : side + 2;
int targetUS;

void goToSafeWallDistance()
{
  int movementSpeed = 5;
  double frontLeftIR, frontRightIR;
  getFrontIR(&frontLeftIR, &frontRightIR);
  while((frontLeftIR > MAX_IR_RANGE) || (frontRightIR > MAX_IR_RANGE))
  {
  	// printf("greater than max\n");
    set_motors(movementSpeed,movementSpeed);
    getFrontIR(&frontLeftIR, &frontRightIR);
  }
  set_motors(0,0);
  while((frontLeftIR < MIN_IR_RANGE) || (frontRightIR < MIN_IR_RANGE))
  {
  	// printf("less than min\n");
    set_motors(-movementSpeed,-movementSpeed);
    getFrontIR(&frontLeftIR, &frontRightIR);
  }
  set_motors(0,0);
}

void adjustAngle()
{
  double frontLeftIR, frontRightIR;
  set_ir_angle(LEFT, ADJUST_IR_ANGLE);
  set_ir_angle(RIGHT, -ADJUST_IR_ANGLE);    
  usleep(SLEEPTIME);
  getFrontIR(&frontLeftIR, &frontRightIR);
  double sensorDifference;

  goToSafeWallDistance();

  while(fabs(frontRightIR - frontLeftIR) > SENSOR_THRESHOLD)
  {
    getFrontIR(&frontLeftIR, &frontRightIR);
    sensorDifference = frontLeftIR - frontRightIR;
    if(sensorDifference < -10)
      sensorDifference = -10;
    else if(sensorDifference < 0)
      sensorDifference = -2;
    else if(sensorDifference > 10)
      sensorDifference = 10;
    else if(sensorDifference > 0)
      sensorDifference = 2;
    set_motors(sensorDifference, -sensorDifference);
  }
  set_motors(0,0);
}

void adjustWallDistance()
{
  double ultraSound;
  targetUS = targetIR + US_OFFSET;
  ultraSound = getUSValue();
  while(fabs(ultraSound - targetUS) > SENSOR_THRESHOLD)
  {
    ultraSound = get_us_dist();
    int movementSpeed = ultraSound - targetUS;
    if(movementSpeed < -10)
    	movementSpeed = -10;
    else if(movementSpeed < 0)
    	movementSpeed = -6;
    else if(movementSpeed > 10)
    	movementSpeed = 10;
    else if(movementSpeed > 0)
    	movementSpeed = 6;

    set_motors(movementSpeed,movementSpeed);
  }
  set_motors(0,0);
}




void correctPositionPerpendicularWalls()
{
  adjustAngle();
  adjustWallDistance();

  set_ir_angle(LEFT, -45);
  set_ir_angle(RIGHT, 45); 
  usleep(SLEEPTIME);

  double sideLeft, sideRight;
  double frontLeft, frontRight;
  getSideIR(&sideLeft, &sideRight);
  getFrontIR(&frontLeft, &frontRight);

  if((sideLeft < DETECT_WALL_DISTANCE) || (frontLeft < DETECT_WALL_DISTANCE))
  {
  	turnByAngleDegree(-90.00);
  	usleep(SLEEPTIME);
  	adjustAngle();
    usleep(SLEEPTIME);
  	adjustWallDistance();
    usleep(SLEEPTIME);
    turnByAngleDegree(90.00);
    usleep(SLEEPTIME);

    // int leftValue = ((double)(sideLeft + frontLeft))/2;
    // updateCoordinates(currentSector, ultraSound, leftValue, LEFT_SIDE);
  }

  else if((sideRight < DETECT_WALL_DISTANCE) || (frontRight < DETECT_WALL_DISTANCE))
  {
  	turnByAngleDegree(90.00);
  	usleep(SLEEPTIME);
  	adjustAngle();
    usleep(SLEEPTIME);
  	adjustWallDistance();
    usleep(SLEEPTIME);
    turnByAngleDegree(-90.00);
    usleep(SLEEPTIME);
    // int rightValue = ((double)(sideRight + frontRight))/2;
    // updateCoordinates(currentSector, ultraSound,rightValue,RIGHT_SIDE);
  }
}

void correctPositionDeadEnd()
{
  adjustAngle();
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);
  turnByAngleDegree(-90.00);
  usleep(SLEEPTIME);
  adjustAngle();
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);
  turnByAngleDegree(180.00);
  usleep(SLEEPTIME);
  adjustAngle();
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);
  turnByAngleDegree(90.00);
  usleep(SLEEPTIME);
  // xPos = x * SECTOR_WIDTH;
  // yPos = y * SECTOR_WIDTH;
}

void correctPositionParallelWalls()
{
  turnByAngleDegree(90.00);
  usleep(SLEEPTIME);
  adjustAngle();
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);
  turnByAngleDegree(180.00);
  usleep(SLEEPTIME);
  adjustAngle();
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);
  turnByAngleDegree(90.00);
  usleep(SLEEPTIME);
}

void singleWallCase(XY currentSector)
{
  int x = currentSector.x;
  int y = currentSector.y;
  double angleToTurn;
  turnByAngleDegree(-convertToDegrees(bearing));
  // printf("OBRACAM O: %.2f", -bearing);
  // int z;
  // scanf("%d",&z);
  if(maze[x][y].northWall)
  {
	angleToTurn = 0.0;
  }
  else if(maze[x][y].southWall)
  {
  	angleToTurn = 180.0;
  }
  else if(maze[x][y].eastWall)
  {
  	angleToTurn = 90.0;
  }
  else if(maze[x][y].westWall)
  {
  	angleToTurn = -90.0;
  }

  if(angleToTurn > 180)
  { 
 	angleToTurn -= 360;
  }
  else if(angleToTurn < -180)
  {
  	angleToTurn += 360;
  }


	turnByAngleDegree(angleToTurn);

  
  // scanf("%d",&z);

  	adjustAngle();
  	adjustWallDistance();
  	turnByAngleDegree(-angleToTurn);
	bearing = 0;
	printf("\t1angleToTurn: %.2f", angleToTurn);
	printf("\tBEARING: %.2f", bearing);
	int p;
	scanf("%d", &p);
}

void correctPosition(XY currentSector)
{
  int x = currentSector.x;
  int y = currentSector.y;
  int wallCount = maze[x][y].eastWall + maze[x][y].westWall + maze[x][y].southWall + maze[x][y].northWall;
  int xAxisWalls = maze[x][y].eastWall + maze[x][y].westWall;
  int yAxisWalls = maze[x][y].southWall + maze[x][y].northWall;
  //case 1: 3 sciany -dead end
  // case 2: 2 przylegle - swastyka, etc
  //case 3: 2 rownolegle sciany
  printf("**************************\n");
  printf("SOUTH KURWA WALL: %d\n", maze[x][y].southWall);
    printf("**************************\n");
  printf("wall count: %d \n", wallCount);
  if(wallCount == 3)
  {	
  	printf("CASE 1\n");
  	correctPositionDeadEnd();
  	updateRobotPosition();
  	printf("unupdated: X: %f, Y: %f\n",xPos,yPos);
  	xPos = x * SECTOR_WIDTH;
  	yPos = y * SECTOR_WIDTH;
  }

  else if((xAxisWalls) && (yAxisWalls))
  {
  	printf("CASE 2\n");
	correctPositionPerpendicularWalls();
	updateRobotPosition();
    printf("unupdated: X: %f, Y: %f\n",xPos,yPos);
  	xPos = x * SECTOR_WIDTH;
  	yPos = y * SECTOR_WIDTH;
  }
  else if(wallCount == 2) // z jakiegos powodu robot tego nie lubi i w ogole sie gubi
  {
  	printf("CASE 3\n");
  	correctPositionParallelWalls();
  	updateRobotPosition();
    printf("unupdated: X: %f, Y: %f\n",xPos,yPos);
  	if((convertToDegrees(bearing) < 15 || convertToDegrees(bearing) > 345) || (convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165))
  	{
      xPos = x * SECTOR_WIDTH;
    }
    
    if((convertToDegrees(bearing) < 105 && convertToDegrees(bearing) > 75) || (convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165))
    {
      yPos = y * SECTOR_WIDTH;
    }
    printf("after update: x: %.2f y: %.2f\n",xPos,yPos);
  }
  else if(wallCount == 1)
  {
   singleWallCase(currentSector);
  }
}

void centerStartingPosition()
{
  double frontLeftIR, frontRightIR, sideLeftIR, sideRightIR;
  turnByAngleDegree(180.00);
  adjustAngle();
  set_ir_angle(LEFT, -45);
  set_ir_angle(RIGHT, 45);    
  usleep(SLEEPTIME);
  getFrontIR(&frontLeftIR, &frontRightIR);
  getSideIR(&sideLeftIR, &sideRightIR);
  targetIR = (frontLeftIR + frontRightIR + sideLeftIR + sideRightIR)/4;
  adjustWallDistance();
  turnByAngleDegree(90.00);
  adjustAngle();
  adjustWallDistance();
  turnByAngleDegree(180.00);
  adjustAngle();
  printf("I SHOULDNT MOVE IF I MOVE CORRECT US_OFFSET!!\n");
  adjustWallDistance();
  turnByAngleDegree(-90.00);

  bearing = 0; // so that the bearing would be affected by this movement (so we would know it's slightly off ( important!))
  updateRobotPosition();
  
  // bearing = 0;
  xPos = 0;
  yPos = -SECTOR_WIDTH;
  updateRobotPosition();






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
  // printf("\n BEARING %.2f", bearing); // debug
  while ((fabs(remainingDistance)) > 3) // value to change
  {	
    set_point(xPos,yPos);
    log_trail();
      printf("\n BEARING %.2f", bearing); // debug
    // printf("\nCurrent X: %f\t current Y: %f \t current bearing: %f \n\n",xPos,yPos,bearing); // debug
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
    	while(fabs(requiredAngleChange) > 0.03)
    	{
		  set_point(xPos,yPos);
		  log_trail();
          int speed = requiredAngleChange*35.0;
          if(speed < -15)
    	    speed = -15;
    	  else if(speed < 0)
    	    speed = -8;
    	  else if(speed > 15)
    	    speed = 15;
   		  else if(speed > 0)
    	    speed = 8;
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
          set_motors(speed,-speed);
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
	set_ir_angle(LEFT, 90);
	set_ir_angle(RIGHT, -90);
	sleep(2);
}

void bumpers()
{
	if(check_bump(LEFT) == 1 || check_bump(RIGHT) == 1)
	{
		printf("\t\tBUMPER HIT\n");
		set_motors(-10,-10);
		usleep(1000000);
		set_motors(0,0);
	}
}

