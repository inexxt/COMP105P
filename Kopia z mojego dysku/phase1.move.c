#include "phase1.move.h"
#include "phase1.map.h"
#include "basicMovement.h"


//TODO: wywalic updaterobotposition();
//TODO: update gotoXY();

#define US_OFFSET 6.5 //difference between av. IR and US (simulator - 2, real robot ~ 6) - zeby latwiej bylo zmieniac
#define ADJUST_IR_ANGLE 25
#define MIN_IR_RANGE 12
#define MAX_IR_RANGE 36
#define SENSOR_THRESHOLD 0.45

#define MIN_US_DIST 12
#define OPTIMAL_US_DIST 16
#define MAX_US_DIST 20

double xPos, yPos, bearing;
int targetIR = SECTOR_WIDTH/2 - ROBOT_WIDTH/2; //INICJALIZACJA TYLKO -> POTEM SIE WARTOSC ZMIENIA/POPRAWIA // wiem ze mozna jako lokanlna, ale wg. mnie w ten sposob jest bardziej czytelne i dostepne
// dotyczy side sensorow, front : side + 2;
int targetUS;
double targetWallReadings;
double sensorDifference;

double localSectorWidth = SECTOR_WIDTH;


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
    // if((frontLeftIR < MIN_IR_RANGE) || (frontRightIR < MIN_IR_RANGE))
    // {
    //   set_motors(0,0);
    //   turnByAngleDegree(-15.00);
    //   usleep(SLEEPTIME);
    // }
  }
  set_motors(0,0);
  while((frontLeftIR < MIN_IR_RANGE) || (frontRightIR < MIN_IR_RANGE))
  {
  	// printf("less than min\n");
    set_motors(-movementSpeed,-movementSpeed);
    getFrontIR(&frontLeftIR, &frontRightIR);
    // if((frontLeftIR < MAX_IR_RANGE) || (frontRightIR < MAX_IR_RANGE))
    // {
    //   set_motors(0,0);
    //   turnByAngleDegree(-15.00);
    //   usleep(SLEEPTIME);
    // }
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
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);

  set_ir_angle(LEFT, -45);
  set_ir_angle(RIGHT, 45); 
  usleep(SLEEPTIME);

  double sideLeft, sideRight;
  double frontLeft, frontRight;
  getSideIR(&sideLeft, &sideRight);
  getFrontIR(&frontLeft, &frontRight);

  if((sideLeft < DETECT_WALL_DISTANCE) && (frontLeft < DETECT_WALL_DISTANCE))
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

  else if((sideRight < DETECT_WALL_DISTANCE) && (frontRight < DETECT_WALL_DISTANCE))
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
//   turnByAngleDegree(180.00);
//   usleep(SLEEPTIME);
//   adjustAngle();
//   usleep(SLEEPTIME);
//   adjustWallDistance();
//   usleep(SLEEPTIME);
  turnByAngleDegree(-90.00);
  usleep(SLEEPTIME);
}

void correctPositionParallelWalls()
{
  turnByAngleDegree(90.00);
  usleep(SLEEPTIME);
  adjustAngle();
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);
//   turnByAngleDegree(180.00);
//   usleep(SLEEPTIME);
//   adjustAngle();
//   usleep(SLEEPTIME);
//   adjustWallDistance();
//   usleep(SLEEPTIME);
  turnByAngleDegree(-90.00);
  usleep(SLEEPTIME);
}

void singleWallCase(XY currentSector)
{
  int x = currentSector.x;
  int y = currentSector.y;
  double angleToTurn = -(convertToDegrees(bearing));

  if(maze[x][y].northWall)
  {
	  angleToTurn += 0.0;
  }
  else if(maze[x][y].southWall)
  {
  	angleToTurn += 180.0;
  }
  else if(maze[x][y].eastWall)
  {
  	angleToTurn += 90.0;
  }
  else if(maze[x][y].westWall)
  {
  	angleToTurn += -90.0;
  }

  while(angleToTurn > 180.0)
  { 
 	  angleToTurn -= 360.0;
  }
  while(angleToTurn < -180.0)
  {
  	angleToTurn += 360.0;
  }

  usleep(SLEEPTIME);
	turnByAngleDegree(angleToTurn);
  usleep(SLEEPTIME);
 	adjustAngle();
  usleep(SLEEPTIME);
  adjustWallDistance();
  usleep(SLEEPTIME);
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
  printf("wall count: %d \n", wallCount);
  int updated = 0;
  printf("PRE- UPDATED X: %.2f Y: %.2f\n",xPos,yPos);
  if(wallCount == 3)
  {	
    updated = 1;
  	printf("CASE 1\n");
  	correctPositionDeadEnd();
  	updateRobotPosition();
  	xPos = x * SECTOR_WIDTH;
  	yPos = y * SECTOR_WIDTH;
  }

  else if((xAxisWalls) && (yAxisWalls))
  {
    updated = 1;
  	printf("CASE 2\n");
	correctPositionPerpendicularWalls();
	updateRobotPosition();
  	xPos = x * SECTOR_WIDTH;
  	yPos = y * SECTOR_WIDTH;
  }
  else if(wallCount == 2) // z jakiegos powodu robot tego nie lubi i w ogole sie gubi
  {
    updated = 1;
  	printf("CASE 3\n");
  	correctPositionParallelWalls();
  	updateRobotPosition();
  	if(yAxisWalls)
  	{
      xPos = x * SECTOR_WIDTH;
    }
    
    else if(xAxisWalls)
    {
      yPos = y * SECTOR_WIDTH;
    }
  }
  else if(wallCount == 1)
  {
    updated = 1;
    singleWallCase(currentSector);
    updateRobotPosition();
    if(yAxisWalls)
    {
      yPos = y * SECTOR_WIDTH;
    }
    
    if(xAxisWalls)
    {
      xPos = x * SECTOR_WIDTH;
    }
  }
  printf("JUST UPDATED X: %.2f Y: %.2f\n",xPos,yPos);
  if(updated)
  {
    updated = 0;
    if(convertToDegrees(bearing) < 15 || convertToDegrees(bearing) > 345) 
      bearing = 0;
    else if(convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165)
      bearing = M_PI; 
    else if(convertToDegrees(bearing) < 105 && convertToDegrees(bearing) > 75) 
      bearing = M_PI/2;
    else if(convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165)
      bearing = (-(M_PI/2)); 
  }
  usleep(SLEEPTIME);
}

void centerStartingPosition()
{
  double frontLeftIR, frontRightIR, sideLeftIR, sideRightIR;
  turnByAngleDegree(180.00);
  usleep(SLEEPTIME);
  adjustAngle();
  usleep(SLEEPTIME);
  set_ir_angle(LEFT, -45);
  set_ir_angle(RIGHT, 45);    
  usleep(SLEEPTIME);
  getFrontIR(&frontLeftIR, &frontRightIR);
  getSideIR(&sideLeftIR, &sideRightIR);
  targetIR = (frontLeftIR + frontRightIR + sideLeftIR + sideRightIR)/4;
  adjustWallDistance();
  usleep(SLEEPTIME);
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
  printf("I SHOULDNT MOVE IF I MOVE CORRECT US_OFFSET!!\n");
  adjustWallDistance();
    usleep(SLEEPTIME);
    // updateRobotPosition();
    // bearing = M_PI/2;
    usleep(SLEEPTIME);
      turnByAngleDegree(-90.00);
        usleep(SLEEPTIME);

  bearing = 0; // so that the bearing would be affected by this movement (so we would know it's slightly off ( important!))
  updateRobotPosition();
  
  // bearing = 0;
  xPos = 0;
  yPos = -SECTOR_WIDTH;
  updateRobotPosition();
}


void bumpers()
{
  if(check_bump(LEFT) == 1 && check_bump(RIGHT) == 1)
  {
    printf("\t\tBUMPER HIT\n");
    set_motors(-10,-10);
    usleep(2000000);
    set_motors(0,0);
  }
  else if(check_bump(LEFT) == 1)
  {
    set_motors(-5,-15);
    usleep(2000000);
    set_motors(0,0);
  }
  else if(check_bump(RIGHT) == 1)
  {
    set_motors(-15,-5);
    usleep(2000000);
    set_motors(0,0);
  }
}


int isHeadingStraight()
{
  if(convertToDegrees(bearing) < 2 || convertToDegrees(bearing) > 358) 
  {
    return 1;
  }
    
  if(convertToDegrees(bearing) < 92 && convertToDegrees(bearing) > 88) 
  {
    return 1;
  }
    
  if(convertToDegrees(bearing) < 182 && convertToDegrees(bearing) > 178) 
  {
    return 1;
  }
    
  if(convertToDegrees(bearing) < 272 && convertToDegrees(bearing) > 268) 
  {
    return 1;
  }

  return 0;
}


void goToXY(XY destination, int phase)
{
  double xDifference;
  double yDifference;
  double requiredAngleChange;
  double remainingDistance = 10000.00;

  double xCoordinate;
  double yCoordinate;

  int canUpdate = 1;
  double targetThreshold;
  double angleThreshold;

  double phase1SpeedConstant = 30.0; 
  double phase2SpeedConstant = 50.0;
  int baseSpeed;
  int turnSpeed;

  if(phase != 2)
  {
    targetThreshold = 1;
    angleThreshold = 0.05;
    baseSpeed = MEDIUM_SPEED;
  }
  
  else
  {
    targetThreshold = 5;
    angleThreshold = 0.75;
    baseSpeed = 2 * MEDIUM_SPEED;
  }


  if(maze[destination.x][destination.y].xCenter == 0)
  {
    xCoordinate = destination.x * localSectorWidth;
    maze[destination.x][destination.y].xCenter = xCoordinate;
  }
  else
    xCoordinate = maze[destination.x][destination.y].xCenter;
  if(maze[destination.x][destination.y].yCenter == 0)
  {
    yCoordinate = destination.y * localSectorWidth;
    maze[destination.x][destination.y].yCenter = yCoordinate;
  }
  else
    yCoordinate = maze[destination.x][destination.y].yCenter;




  //printf("\nCoordinates: %f\t%f", xCoordinate,yCoordinate); // debug
  // printf("\n BEARING %.2f", bearing); // debug
  while (((fabs(remainingDistance)) > targetThreshold ) || (fabs(remainingDistance > 2) && isHeadingStraight() == 0)) // value to change
  {	

    //set_point(xPos,yPos);
    log_trail();
      // printf("\n BEARING %.2f", bearing); // debug
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

    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);


	  if(fabs(requiredAngleChange) > 1.2) 
    {
    	while(fabs(requiredAngleChange) > angleThreshold)
    	{
		    log_trail();
        
        
        if(turnSpeed < -(baseSpeed/2))
    	    turnSpeed = -(baseSpeed/2);
    	  else if(turnSpeed < 0)
    	    turnSpeed = -(baseSpeed/8);
    	  else if(turnSpeed > (baseSpeed/2))
    	    turnSpeed = (baseSpeed/2);
   		  else if(turnSpeed > 0)
    	    turnSpeed = (baseSpeed/8);
        
        updateRobotPosition(); 
        requiredAngleChange = atan2(xDifference,yDifference) - bearing;
        if(phase != 2)
          turnSpeed = requiredAngleChange * phase1SpeedConstant;    
        else
          turnSpeed = requiredAngleChange * phase2SpeedConstant;
        while(requiredAngleChange > (M_PI))
        {
          requiredAngleChange -= (2 * M_PI);
        }
        while(requiredAngleChange < (-M_PI))
        {
          requiredAngleChange += (2 * M_PI);
        }
        set_motors(turnSpeed,-turnSpeed);
    	}
    }
    else
    {
      bumpers();

      updateRobotPosition(); 
      int leftSpeed = baseSpeed + baseSpeed * requiredAngleChange;
      int rightSpeed = baseSpeed - baseSpeed * requiredAngleChange;
      if(phase != 2)
      {
        if(remainingDistance < 20)
        {
          leftSpeed /= 2;
          rightSpeed /= 2;
        }
        if(remainingDistance < 10)
        {
          leftSpeed /= 2;
          rightSpeed /= 2;
        }
      }
      if(phase == 1)
      {
        int frontLeftReading, frontRightReading, sideLeftReading, sideRightReading; 
    
        double leftOffset = 0;
        double rightOffset = 0;
        double offSet = 0;;
        double currentSensorDifference;
        if(canUpdate)
        {
          get_front_ir_dists(&frontLeftReading, &frontRightReading);
          get_side_ir_dists(&sideLeftReading, &sideRightReading);
          double leftWallValue = (frontLeftReading + sideLeftReading)/2;
          double rightWallValue = (frontRightReading + sideRightReading)/2;

          if(frontLeftReading < DETECT_WALL_DISTANCE && sideLeftReading < DETECT_WALL_DISTANCE)
          {
            leftOffset = (targetWallReadings - leftWallValue);
            currentSensorDifference = frontLeftReading - sideLeftReading;
            // printf("LEFT OFFSET: %f\n", leftOffset);
          }
          if(frontRightReading < DETECT_WALL_DISTANCE && sideRightReading < DETECT_WALL_DISTANCE)
          {
            rightOffset = -(targetWallReadings - rightWallValue);
            currentSensorDifference = (currentSensorDifference + (frontRightReading + sideRightReading))/2.0;
            // printf("RIGHT OFFSET: %f\n", rightOffset);
          }
          if(leftOffset)
            offSet = leftOffset;
          else
            offSet = rightOffset;
        }


        if(canUpdate && offSet) // ((fabs(currentSensorDifference) - 2.5) < sensorDifference) &&
        {  
          offSet /= 3;
          if(convertToDegrees(bearing) < 15 || convertToDegrees(bearing) > 345) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
          
            maze[destination.x][destination.y].xCenter += offSet;
            xCoordinate = maze[destination.x][destination.y].xCenter;

            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
            printf("IR CHANGE 1\n");
          }
        
          if(convertToDegrees(bearing) < 105 && convertToDegrees(bearing) > 75) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

            maze[destination.x][destination.y].yCenter -= offSet;
            yCoordinate = maze[destination.x][destination.y].yCenter;

            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                        printf("IR CHANGE 2\n");
          }
            
          if(convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
           
            maze[destination.x][destination.y].xCenter -= offSet;
            xCoordinate = maze[destination.x][destination.y].xCenter;
          
            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                        printf("IR CHANGE 3\n");
          }
            
          if(convertToDegrees(bearing) < 285 && convertToDegrees(bearing) > 255) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
          
            maze[destination.x][destination.y].yCenter += offSet;
            yCoordinate = maze[destination.x][destination.y].yCenter;
            
            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                        printf("IR CHANGE 4\n");
          }
          canUpdate = 0;
          // localSectorWidth += 
        }

        int usReading = get_us_dist();
        if((usReading < MIN_US_DIST) || ((usReading > MAX_US_DIST) && (usReading < (0.35*localSectorWidth))))
        {
          printf("\n\n US READING: %d\n\n", usReading);
          int distanceToMove = (OPTIMAL_US_DIST - usReading);
          while(usReading < OPTIMAL_US_DIST)
          {
            usReading = get_us_dist();
            int moveSpeed = usReading - OPTIMAL_US_DIST;
            set_motors(moveSpeed, moveSpeed);
          }

          localSectorWidth += (distanceToMove/8.0);

          if(convertToDegrees(bearing) < 15 || convertToDegrees(bearing) > 345) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
        
            maze[destination.x][destination.y].yCenter -= distanceToMove;
            xCoordinate = maze[destination.x][destination.y].yCenter;

            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                        printf("US CHANGE 1\n");
          }
        
          if(convertToDegrees(bearing) < 105 && convertToDegrees(bearing) > 75) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

            maze[destination.x][destination.y].xCenter -= distanceToMove;
            yCoordinate = maze[destination.x][destination.y].xCenter;

            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                                    printf("US CHANGE 2\n");

          }
            
          if(convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
           
            maze[destination.x][destination.y].yCenter -= distanceToMove;
            xCoordinate = maze[destination.x][destination.y].yCenter;
          
            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                                    printf("US CHANGE 3\n");

          }
            
          if(convertToDegrees(bearing) < 285 && convertToDegrees(bearing) > 255) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
          
            maze[destination.x][destination.y].xCenter -= distanceToMove;
            yCoordinate = maze[destination.x][destination.y].xCenter;
            
            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                                    printf("US CHANGE 4\n");

          }
            remainingDistance = 0;
        }


      }


      set_motors((leftSpeed),(rightSpeed));
    }
  }

 



  set_motors(0,0);

}

void endPhase1()
{
  printf("FINAL SECTOR_WIDTH: %f\n", localSectorWidth);

  turnByAngleDegree(180.0);
	set_ir_angle(LEFT, 90);
	set_ir_angle(RIGHT, -90);
	sleep(1);
  set_ir_angle(LEFT, 45);
  set_ir_angle(RIGHT, -45);
  sleep(1);
}
