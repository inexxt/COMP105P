#include "phase1.move.h"
#include "phase1.map.h"
#include "basicMovement.h"

#define ADJUST_IR_ANGLE 25
#define SENSOR_THRESHOLD 0.45

#define US_THRESHOLD 1
#define IR_THRESHOLD 1

#define IR_ADJUSTMENT 1
#define US_ADJUSTMENT 1


double xPos, yPos, bearing; // extern global
double targetWallReadings; // extern global
double sensorDifference; // extern global

double localSectorWidth = SECTOR_WIDTH;

double optimal_US_dist;
double min_US_dist;
double max_US_dist;

Sector maze[MAZE_WIDTH][MAZE_HEIGHT];


void adjustAngle()
{
  double frontLeftIR, frontRightIR;
  set_ir_angle(LEFT, ADJUST_IR_ANGLE);
  set_ir_angle(RIGHT, -ADJUST_IR_ANGLE);    
  usleep(SLEEPTIME);
  getFrontIR(&frontLeftIR, &frontRightIR);
  double IR_difference;

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


void bumpers()
{
  if(check_bump(LEFT) == 1 && check_bump(RIGHT) == 1)
  {
    printf("BOTH BUMPER HIT\n");
    set_motors(-10,-10);
    usleep(2000000);
    set_motors(0,0);
  }
  else if(check_bump(LEFT) == 1)
  {
    printf("LEFT BUMPER HIT\n");
    set_motors(-5,-15);
    usleep(2000000);
    set_motors(0,0);
  }
  else if(check_bump(RIGHT) == 1)
  {
    printf("RIGHT BUMPER HIT\n");
    set_motors(-15,-5);
    usleep(2000000);
    set_motors(0,0);
  }
}

void IR_targetAdjustment(XY destination)
{
  set_motors(0,0);
  double frontLeftReading, frontRightReading, sideLeftReading, sideRightReading; 

  double leftOffset = 0;
  double rightOffset = 0;
  double offSet = 0;
  double currentSensorDifference = 0;
 

  getFrontIR(&frontLeftReading, &frontRightReading);
  getSideIR(&sideLeftReading, &sideRightReading);


  if(frontLeftReading < DETECT_WALL_DISTANCE && sideLeftReading < DETECT_WALL_DISTANCE)
  {
    currentSensorDifference = frontLeftReading - sideLeftReading;
    while(fabs(currentSensorDifference - sensorDifference) > IR_THRESHOLD)
    {
      currentSensorDifference = get_front_ir_dist(LEFT) - get_side_ir_dist(LEFT);
      double relativeDifference = currentSensorDifference - sensorDifference;
      if(relativeDifference > 5)
        relativeDifference = 5;
      if(relativeDifference < -5)
        relativeDifference = -5;
      set_motors(-relativeDifference,relativeDifference);
    }
    set_motors(0,0);
  }

  if(frontRightReading < DETECT_WALL_DISTANCE && sideRightReading < DETECT_WALL_DISTANCE)
  {
    currentSensorDifference = frontRightReading - sideRightReading;
    while(fabs(currentSensorDifference - sensorDifference) > IR_THRESHOLD)
    {
      currentSensorDifference = get_front_ir_dist(RIGHT) - get_side_ir_dist(RIGHT);
      double relativeDifference = currentSensorDifference - sensorDifference;
      if(relativeDifference > 5)
        relativeDifference = 5;
      if(relativeDifference < -5)
        relativeDifference = -5;
      set_motors(relativeDifference,-relativeDifference);
    }
    set_motors(0,0);
  }

  getFrontIR(&frontLeftReading, &frontRightReading);
  getSideIR(&sideLeftReading, &sideRightReading);
  double leftWallValue = (frontLeftReading + sideLeftReading)/2;
  double rightWallValue = (frontRightReading + sideRightReading)/2;
  leftOffset = (targetWallReadings - leftWallValue); // o ile za blisko lewa sciana - o ile sie oddalic
  rightOffset = -(targetWallReadings - rightWallValue); // o ile za blisko lewa sciana - o ile sie oddalic 


  double minOffSet = 3;
  double maxOffSet = 12.0;

  
  printf("LEFT OFFSET: %f, RIGHT OFFSET: %f\n",leftOffset,rightOffset);

  if(fabs(leftOffset) < maxOffSet && fabs(rightOffset) < maxOffSet)
  {
    if(fabs(leftOffset) > fabs(rightOffset))
      offSet = rightOffset;
    else
      offSet = leftOffset;
  }
  else if(fabs(leftOffset) < maxOffSet)
    offSet = leftOffset;
  else if(fabs(rightOffset) < maxOffSet)
    offSet = rightOffset;
  else
    offSet = 0;

  printf("OFFSET: %f\n",offSet);
  printf("\n\nPREUPDATED: Currently at: %f %f, going to: %f %f\n", xPos,yPos,maze[destination.x][destination.y].xCenter, maze[destination.x][destination.y].yCenter);
  int i;
  if(convertToDegrees(bearing) < 10 || convertToDegrees(bearing) > 350) 
  {
    printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

    // if(fabs(offSet) > minOffSet)
    //   maze[destination.x][destination.y].xCenter += offSet;
    // else
    //   maze[destination.x][destination.y].xCenter = xPos;

    maze[destination.x][destination.y].xCenter = xPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].xCenter += offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[destination.x][i].xCenter = maze[destination.x][destination.y].xCenter;
    }

    printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
    printf("IR CHANGE facing North\n");
  }

  if(convertToDegrees(bearing) < 100 && convertToDegrees(bearing) > 80) 
  {
    printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

    // if(fabs(offSet) > minOffSet){
    //   maze[destination.x][destination.y].yCenter -= offSet;
    //   printf("overwrite1\n");
    // }
    // else{
    //   maze[destination.x][destination.y].yCenter = yPos;
    //   printf("overwrite2\n");
    // }

    maze[destination.x][destination.y].yCenter = yPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].yCenter -= offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
    }

    printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                printf("IR CHANGE facing East\n");
  }
    
  if(convertToDegrees(bearing) < 190 && convertToDegrees(bearing) > 170) 
  {
    printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
   
    // if(fabs(offSet) > minOffSet)
    //   maze[destination.x][destination.y].xCenter -= offSet;
    // else
    //   maze[destination.x][destination.y].xCenter = xPos;

    maze[destination.x][destination.y].xCenter = xPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].xCenter -= offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[destination.x][i].xCenter = maze[destination.x][destination.y].xCenter;
    }

    printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                printf("IR CHANGE facing South\n");
  }
    
  if(convertToDegrees(bearing) < 280 && convertToDegrees(bearing) > 260) 
  {
    printf("Changing %f,%f to ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
    // if(fabs(offSet) > minOffSet)
    //   maze[destination.x][destination.y].yCenter += offSet;
    // else
    //   maze[destination.x][destination.y].yCenter = yPos;

    maze[destination.x][destination.y].yCenter = yPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].yCenter += offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
    }

    printf("%f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
                printf("IR CHANGE facing West\n");
  }
  printf("POSTUPDATED: Currently at: %f %f, going to: %f %f\n\n", xPos,yPos,maze[destination.x][destination.y].xCenter, maze[destination.x][destination.y].yCenter);

  set_motors(0,0);
}


void US_targetAdjustment(XY destination)
{
  double usReading = getUSValue(); // dla dodatkowej dokladnosci, zeby sie upewnic czy na pewno ma wejsc w petle
  if((usReading < min_US_dist) || ((usReading > max_US_dist) && (usReading < (1.6*max_US_dist))))
  {
    double distanceToMove = optimal_US_dist - usReading; // ile sie odsunac od sciany
    int ultraSound = (int)round(usReading);
 
    if(usReading > max_US_dist)
      adjustAngle();


    while(fabs(optimal_US_dist - ultraSound) > US_THRESHOLD)
    {
      ultraSound = get_us_dist();
      int movementSpeed = (ultraSound - optimal_US_dist);
      if(movementSpeed < -10)
        movementSpeed = -10;
      else if(movementSpeed > -3 && movementSpeed <= 0)
        movementSpeed = -2;
      else if(movementSpeed < 3 && movementSpeed >= 0)
        movementSpeed = 2;
      else if(movementSpeed > 10)
        movementSpeed = 10;
      set_motors(movementSpeed,movementSpeed);
    }
    set_motors(0,0);


// west < east < south < north

    int i;
    if(convertToDegrees(bearing) < 10 || convertToDegrees(bearing) > 350) 
    {
      maze[destination.x][destination.y].yCenter -= distanceToMove;
      for(i = 0; i <= 3; i++)
      {
        maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
      }
    }
  
    if(convertToDegrees(bearing) < 100 && convertToDegrees(bearing) > 80) 
    {
      maze[destination.x][destination.y].xCenter -= distanceToMove;
      for(i = 0; i <= 3; i++)
      {
        maze[destination.x][i].xCenter = maze[destination.x][destination.y].xCenter;
      }
    }
      
    if(convertToDegrees(bearing) < 190 && convertToDegrees(bearing) > 170) 
    {
      maze[destination.x][destination.y].yCenter += distanceToMove;
      for(i = 0; i <= 3; i++)
      {
        maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
      }
    }
      
    if(convertToDegrees(bearing) < 280 && convertToDegrees(bearing) > 260) 
    {
      maze[destination.x][destination.y].xCenter += distanceToMove;
      for(i = 0; i <= 3; i++)
      {
        maze[destination.x][i].xCenter = maze[destination.x][destination.y].xCenter;
      }
    }
  }
}


void rotateTowardsTarget(double targetX, double targetY, int phase)
{
  double angleThreshold;
  double speedConstant;
  int baseSpeed;
  int turnSpeed;

  if(phase != 2)
  {
    angleThreshold = 0.05;
    baseSpeed = MEDIUM_SPEED;
    speedConstant = 30.0;
  }
  else
  {
    angleThreshold = 0.75;
    baseSpeed = 2.5 * MEDIUM_SPEED;
    speedConstant = 50.0;
  }

  updateRobotPosition(); 
  double xDifference = targetX - xPos;
  double yDifference = targetY - yPos;
  double requiredAngleChange = atan2(xDifference,yDifference) - bearing;

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

    turnSpeed = requiredAngleChange * speedConstant;

    if(turnSpeed < -(baseSpeed/2))
      turnSpeed = -(baseSpeed/2);
    else if(turnSpeed < 0 && turnSpeed > (-(baseSpeed/8)))
      turnSpeed = -(baseSpeed/8);
    else if(turnSpeed > (baseSpeed/2))
      turnSpeed = (baseSpeed/2);
    else if(turnSpeed > 0 && turnSpeed < (baseSpeed/8))
      turnSpeed = (baseSpeed/8);

    set_motors(turnSpeed,-turnSpeed);
  }
  set_motors(0,0);
}


void moveToTarget(double targetX, double targetY, int phase)
{
  double remainingDistance = 1000.00;
  double previousRemainingDistance = 10000.00; 
  double xDifference, yDifference;

  double targetThreshold; 
  double requiredAngleChange;
  int baseSpeed;
  int leftSpeed, rightSpeed;

  if(phase != 2)
  {
    targetThreshold = 1.5;
    baseSpeed = MEDIUM_SPEED;
  }
  else
  {
    targetThreshold = 8;
    baseSpeed = 2.5 * MEDIUM_SPEED;
  }

  while (((fabs(remainingDistance)) > targetThreshold ) && (remainingDistance <= (previousRemainingDistance + targetThreshold)))
  { 
    bumpers();
    log_trail();
    updateRobotPosition(); 
    previousRemainingDistance = remainingDistance;

    xDifference = targetX - xPos;
    yDifference = targetY - yPos;

    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);
    requiredAngleChange = atan2(xDifference,yDifference)- bearing;

    while(requiredAngleChange > (M_PI))
    {
      requiredAngleChange -= (2 * M_PI);
    }
    while(requiredAngleChange < (-M_PI))
    {
      requiredAngleChange += (2 * M_PI);
    }

    leftSpeed = baseSpeed + baseSpeed * requiredAngleChange;
    rightSpeed = baseSpeed - baseSpeed * requiredAngleChange;

    XY currentSector = getCurrentSector();
    if(phase != 2 && maze[currentSector.x][currentSector.y].visited == 0)
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

    if(fabs(requiredAngleChange) > (M_PI/4))
      break;

    set_motors(leftSpeed,rightSpeed);
  }
  set_motors(0,0);
}


void goToXY(XY destination, int phase)
{

  // because C is stupid.
  optimal_US_dist = targetWallReadings + US_OFFSET;
  min_US_dist = optimal_US_dist - 4;
  max_US_dist = optimal_US_dist + 4;
  double targetX, targetY;
  int canUpdate = 1;

  if(maze[destination.x][destination.y].xCenter == 0)
  {
    targetX = destination.x * localSectorWidth;
    maze[destination.x][destination.y].xCenter = targetX;
  }
  else
    targetX = maze[destination.x][destination.y].xCenter;
  
  if(maze[destination.x][destination.y].yCenter == 0)
  {
    targetY = destination.y * localSectorWidth;
    maze[destination.x][destination.y].yCenter = targetY;
  }
  else
    targetY = maze[destination.x][destination.y].yCenter;

  if(destination.x == 0 && destination.y == -1)
  {
    targetX = 0;
    targetY = -localSectorWidth;
  }

  printf("Going to: %f,%f\n",targetX, targetY);

	rotateTowardsTarget(targetX, targetY, phase);
  if(IR_ADJUSTMENT)
  {
    if(phase == 1 && canUpdate)
    {
      IR_targetAdjustment(destination);
      targetX = maze[destination.x][destination.y].xCenter;
      targetY = maze[destination.x][destination.y].yCenter;
    }
  }
  moveToTarget(targetX, targetY, phase);
  if(US_ADJUSTMENT)
  {
    if(phase != 2)
      US_targetAdjustment(destination);
  }
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






// SOME OLD FUNCTIONS WE USED TO USE


/*
#define MIN_IR_RANGE 12
#define MAX_IR_RANGE 36
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


*/