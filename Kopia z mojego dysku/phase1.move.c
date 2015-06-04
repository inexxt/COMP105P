#include "phase1.move.h"
#include "phase1.map.h"
#include "basicMovement.h"


//TODO: wywalic updaterobotposition();
//

//TODO: use adjustAngle and goToSafeWallDistance czy jak to sie nazywalo


#define US_OFFSET 6.5 //difference between av. IR and US (simulator - 2, real robot ~ 6) - zeby latwiej bylo zmieniac
#define ADJUST_IR_ANGLE 25
#define MIN_IR_RANGE 12
#define MAX_IR_RANGE 36
#define SENSOR_THRESHOLD 0.45

// #define MIN_US_DIST 12
// #define OPTIMAL_US_DIST 16
// #define MAX_US_DIST 20

double xPos, yPos, bearing;
int targetIR = SECTOR_WIDTH/2 - ROBOT_WIDTH/2; //INICJALIZACJA TYLKO -> POTEM SIE WARTOSC ZMIENIA/POPRAWIA // wiem ze mozna jako lokanlna, ale wg. mnie w ten sposob jest bardziej czytelne i dostepne
// dotyczy side sensorow, front : side + 2;
int targetUS;
double targetWallReadings; // extern global
double sensorDifference; // extern global

double localSectorWidth = SECTOR_WIDTH;


double optimal_US_dist;
double min_US_dist;
double max_US_dist;



void goToSafeWallDistance()
{
  set_ir_angle(LEFT, ADJUST_IR_ANGLE);
  set_ir_angle(RIGHT, -ADJUST_IR_ANGLE);  

  int movementSpeed = 3;
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
  set_ir_angle(LEFT, -45);
  set_ir_angle(RIGHT, 45); 
}

void adjustAngle()
{
  double frontLeftIR, frontRightIR;
  set_ir_angle(LEFT, ADJUST_IR_ANGLE);
  set_ir_angle(RIGHT, -ADJUST_IR_ANGLE);    
  usleep(SLEEPTIME);
  getFrontIR(&frontLeftIR, &frontRightIR);
  double IR_difference;

  // goToSafeWallDistance();

  while(fabs(frontRightIR - frontLeftIR) > SENSOR_THRESHOLD)
  {
    getFrontIR(&frontLeftIR, &frontRightIR);
    IR_difference = frontLeftIR - frontRightIR;
    if(IR_difference < -10)
      IR_difference = -10;
    else if(IR_difference < 0)
      IR_difference = -2;
    else if(IR_difference > 10)
      IR_difference = 10;
    else if(IR_difference > 0)
      IR_difference = 2;
    set_motors(IR_difference, -IR_difference);
  }
  set_motors(0,0);
  set_ir_angle(LEFT, -45);
  set_ir_angle(RIGHT, 45); 
}

double adjustWallDistance()
{
  double ultraSound;
  ultraSound = getUSValue();

  double adjustment = optimal_US_dist - ultraSound;
  if(ultraSound > max_US_dist)
    adjustAngle();
  while(fabs(ultraSound - optimal_US_dist) > SENSOR_THRESHOLD)
  {
    ultraSound = get_us_dist();
    int movementSpeed = (ultraSound - optimal_US_dist) * 2;
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
  return adjustment;
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
  // because C is stupid.

  optimal_US_dist = targetWallReadings + US_OFFSET;
  min_US_dist = optimal_US_dist - 4;
  max_US_dist = optimal_US_dist + 4;

  
  double xDifference = 1;
  double yDifference = 1;
  double requiredAngleChange;
  double remainingDistance = 10000.00;
  double previousRemainingDistance = 1000000.00;

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
    targetThreshold = 8;
    angleThreshold = 0.75;
    baseSpeed = 2.5 * MEDIUM_SPEED;
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

  if(destination.x == 0 && destination.y == -1)
  {
    xCoordinate = 0;
    yCoordinate = -localSectorWidth;
  }



  //printf("\nCoordinates: %f\t%f", xCoordinate,yCoordinate); // debug
  // printf("\n BEARING %.2f", bearing); // debug

  while (((fabs(remainingDistance)) > targetThreshold ) && (remainingDistance <= (previousRemainingDistance + targetThreshold)))
  {	
    previousRemainingDistance = remainingDistance;
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


	  if(fabs(requiredAngleChange) > 0.8) 
    {
    	while(fabs(requiredAngleChange) > angleThreshold)
    	{
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


        if(phase != 2)
          turnSpeed = requiredAngleChange * phase1SpeedConstant;    
        else
          turnSpeed = requiredAngleChange * phase2SpeedConstant;


        if(turnSpeed < -(baseSpeed/2))
          turnSpeed = -(baseSpeed/2);
        else if(turnSpeed < 0)
          turnSpeed = -(baseSpeed/8);
        else if(turnSpeed > (baseSpeed/2))
          turnSpeed = (baseSpeed/2);
        else if(turnSpeed > 0)
          turnSpeed = (baseSpeed/8);


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

      if(phase == 2)
        canUpdate = 0;

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
            leftOffset = (targetWallReadings - leftWallValue); // o ile za blisko lewa sciana
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


        //printf("\n\n\nVISITED: %d\n\n\n", maze[destination.x][destination.y].visited);
        if(canUpdate && offSet)// && maze[destination.x][destination.y].visited == 0) // ((fabs(currentSensorDifference) - 2.5) < sensorDifference) &&
        {  
          // goto test;
          offSet /= 2;
          if(convertToDegrees(bearing) < 10 || convertToDegrees(bearing) > 350) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
          
            maze[destination.x][destination.y].xCenter += offSet;
            xCoordinate = maze[destination.x][destination.y].xCenter;

            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
            printf("IR CHANGE 1\n");
          }
        
          if(convertToDegrees(bearing) < 100 && convertToDegrees(bearing) > 80) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

            maze[destination.x][destination.y].yCenter -= offSet;
            yCoordinate = maze[destination.x][destination.y].yCenter;

            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                        printf("IR CHANGE 2\n");
          }
            
          if(convertToDegrees(bearing) < 190 && convertToDegrees(bearing) > 170) 
          {
            printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
           
            maze[destination.x][destination.y].xCenter -= offSet;
            xCoordinate = maze[destination.x][destination.y].xCenter;
          
            printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                        printf("IR CHANGE 3\n");
          }
            
          if(convertToDegrees(bearing) < 280 && convertToDegrees(bearing) > 260) 
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

        //happens only when it reaches the target
      }
      test:

      canUpdate = 0;
      set_motors((leftSpeed),(rightSpeed));
    }
  }

  int usReading = get_us_dist();
  if((usReading < min_US_dist) && (phase != 2) || ((usReading >  max_US_dist) && (usReading < (1.3*max_US_dist))))
  {
    // goto test;
    // double tempXPos = xPos;
    // double tempYPos = yPos;
    double distanceToMove = adjustWallDistance();


    // o ile musi sie oddalic



    // updateRobotPosition();
    // xPos = tempXPos;
    // yPos = tempYPos;

    // distanceToMove *= (-1);

    // while(usReading < optimal_US_dist)
    // {
    //   usReading = get_us_dist();
    //   int moveSpeed = usReading - optimal_US_dist;
    //   set_motors(moveSpeed, moveSpeed);
    // }

    // localSectorWidth += (distanceToMove/8.0);

    int i,j;

    if(convertToDegrees(bearing) < 10 || convertToDegrees(bearing) > 350) 
    {
      printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
  

      // for(i = 0; i < 4; i++)
      // {
      //   for(j = 0; j < 4; j++)
      //   {
      //     if(maze[i][j].yCenter == 0)
      //       maze[i][j].yCenter = j * localSectorWidth;
      //     maze[i][j].yCenter += distanceToMove;
      //   }
      // }

      maze[destination.x][destination.y].yCenter -= distanceToMove;


      yCoordinate = maze[destination.x][destination.y].yCenter;

      printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                  printf("US CHANGE 1\n");
    }
  
    if(convertToDegrees(bearing) < 100 && convertToDegrees(bearing) > 80) 
    {
      printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);



      // for(i = 0; i < 4; i++)
      // {
      //   for(j = 0; j < 4; j++)
      //   {
      //     if(maze[i][j].xCenter == 0)
      //       maze[i][j].xCenter = i * localSectorWidth;
      //     maze[i][j].xCenter += distanceToMove;
      //   }
      // }

      maze[destination.x][destination.y].xCenter -= distanceToMove;

      xCoordinate = maze[destination.x][destination.y].xCenter;

      printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                              printf("US CHANGE 2\n");

    }
      
    if(convertToDegrees(bearing) < 190 && convertToDegrees(bearing) > 170) 
    {
      printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
     

      // for(i = 0; i < 4; i++)
      // {
      //   for(j = 0; j < 4; j++)
      //   {
      //     if(maze[i][j].yCenter == 0)
      //       maze[i][j].yCenter = j * localSectorWidth;
      //     maze[i][j].yCenter += distanceToMove;
      //   }
      // }

      maze[destination.x][destination.y].yCenter += distanceToMove;
      yCoordinate = maze[destination.x][destination.y].yCenter;
    
      printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                              printf("US CHANGE 3\n");

    }
      
    if(convertToDegrees(bearing) < 280 && convertToDegrees(bearing) > 260) 
    {
      printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
    

      // for(i = 0; i < 4; i++)
      // {
      //   for(j = 0; j < 4; j++)
      //   {
      //     if(maze[i][j].xCenter == 0)
      //       maze[i][j].xCenter = i * localSectorWidth;
      //     maze[i][j].xCenter += distanceToMove;
      //   }
      // }


      maze[destination.x][destination.y].xCenter += distanceToMove;
      xCoordinate = maze[destination.x][destination.y].xCenter;
      
      printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                              printf("US CHANGE 4\n");

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
