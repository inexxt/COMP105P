#include "phase1.move.h"
#include "phase1.map.h"
#include "basicMovement.h"

#define ADJUST_IR_ANGLE 18
#define SENSOR_THRESHOLD 0.45
#define US_THRESHOLD 1
#define IR_THRESHOLD 1

// for debug in order to disable/enable some position adjustment features
#define IR_ADJUSTMENT 1
#define US_ADJUSTMENT 1


double xPos, yPos, bearing; // extern global
double targetWallReadings; // extern global
double sensorDifference; // extern global
Sector maze[MAZE_WIDTH][MAZE_HEIGHT]; // extern gkibak

double optimal_US_dist;
double min_US_dist;
double max_US_dist;

double startingSectorX = 0.0;
double startingSectorY = -SECTOR_WIDTH;


void adjustAngle()
{
  double frontLeftIR =0, frontRightIR =0;
  set_ir_angle(LEFT, ADJUST_IR_ANGLE);
  set_ir_angle(RIGHT, -ADJUST_IR_ANGLE);    
  usleep(SLEEPTIME);
  getFrontIR(&frontLeftIR, &frontRightIR);
  double IR_difference=0;

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
  double frontLeftReading = 0;
  double frontRightReading = 0;
  double sideLeftReading = 0;
  double sideRightReading = 0;
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

      frontLeftReading = 0;
    frontRightReading = 0;
    sideLeftReading = 0;
    sideRightReading = 0;

  getFrontIR(&frontLeftReading, &frontRightReading);
  getSideIR(&sideLeftReading, &sideRightReading);
  double leftWallValue = (frontLeftReading + sideLeftReading)/2;
  double rightWallValue = (frontRightReading + sideRightReading)/2;
  leftOffset = (targetWallReadings - leftWallValue); // says how much closer than required the robot is to the left wall
  rightOffset = -(targetWallReadings - rightWallValue); // same as above

  double minOffSet = 3;
  double maxOffSet = 12.0;

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

  int i;
  if(destination.y == -1) // to prevent accessing negative array index
  {
    startingSectorX -= offSet;
  }
  else if(convertToDegrees(bearing) < 10 || convertToDegrees(bearing) > 350) 
  {
    printf("Changing %f,%f ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

    maze[destination.x][destination.y].xCenter = xPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].xCenter += offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[destination.x][i].xCenter = maze[destination.x][destination.y].xCenter;
    }

    printf("to %f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
    printf("IR CHANGE facing North\n");
  }

  else if(convertToDegrees(bearing) < 100 && convertToDegrees(bearing) > 80) 
  {
    printf("Changing %f,%f ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

    maze[destination.x][destination.y].yCenter = yPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].yCenter -= offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
    }

    printf("to %f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
    printf("IR CHANGE facing East\n");
  }
    
  else if(convertToDegrees(bearing) < 190 && convertToDegrees(bearing) > 170) 
  {
    printf("Changing %f,%f ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
   
    maze[destination.x][destination.y].xCenter = xPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].xCenter -= offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[destination.x][i].xCenter = maze[destination.x][destination.y].xCenter;
    }

    printf("to %f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
    printf("IR CHANGE facing South\n");
  }
    
  else if(convertToDegrees(bearing) < 280 && convertToDegrees(bearing) > 260) 
  {
    printf("Changing %f,%f ",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);

    maze[destination.x][destination.y].yCenter = yPos;
    if(fabs(offSet) > minOffSet)
      maze[destination.x][destination.y].yCenter += offSet;
    for(i = 0; i <= 3; i++)
    {
      maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
    }

    printf("to %f,%f\n",maze[destination.x][destination.y].xCenter,maze[destination.x][destination.y].yCenter);
    printf("IR CHANGE facing West\n");
  }

  set_motors(0,0);
}


void US_targetAdjustment(XY destination)
{
  double usReading = getUSValue(); // for extra accuracy in order to ensure the loop should be entered
  if((usReading < min_US_dist) || ((usReading > max_US_dist) && (usReading < (1.6*max_US_dist))))
  {
    double distanceToMove = optimal_US_dist - usReading; // how much robot should go away from the wall
    int ultraSound = (int)round(usReading);
 
    if(usReading > max_US_dist)
      adjustAngle(); // if it's too close then there's no point in correcting the angle since the robot would have problems in "catching" the wall

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

    if(destination.x == 5 && destination.y == 5) // arbitrary sector that is used to only correct relative position 
      return;

    int i;
    if(destination.y == -1)
    {
      startingSectorY += distanceToMove;
    }

    else if(convertToDegrees(bearing) < 10 || convertToDegrees(bearing) > 350) 
    {
      maze[destination.x][destination.y].yCenter -= distanceToMove;
      for(i = 0; i <= 3; i++)
      {
        maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
      }
    }
  
    else if(convertToDegrees(bearing) < 100 && convertToDegrees(bearing) > 80) 
    {
      maze[destination.x][destination.y].xCenter -= distanceToMove;
      for(i = 0; i <= 3; i++)
      {
        maze[destination.x][i].xCenter = maze[destination.x][destination.y].xCenter;
      }
    }
      
    else if(convertToDegrees(bearing) < 190 && convertToDegrees(bearing) > 170) 
    {
      maze[destination.x][destination.y].yCenter += distanceToMove;
      for(i = 0; i <= 3; i++)
      {
        maze[i][destination.y].yCenter = maze[destination.x][destination.y].yCenter;
      }
    }
      
    else if(convertToDegrees(bearing) < 280 && convertToDegrees(bearing) > 260) 
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
  // initializing globals 
  optimal_US_dist = targetWallReadings + US_OFFSET;
  min_US_dist = optimal_US_dist - 4;
  max_US_dist = optimal_US_dist + 4;
  double targetX, targetY;

  if(maze[destination.x][destination.y].xCenter == 0)
  {
    targetX = destination.x * SECTOR_WIDTH;
    maze[destination.x][destination.y].xCenter = targetX;
  }
  else
    targetX = maze[destination.x][destination.y].xCenter;
  
  if(maze[destination.x][destination.y].yCenter == 0)
  {
    targetY = destination.y * SECTOR_WIDTH;
    maze[destination.x][destination.y].yCenter = targetY;
  }
  else
    targetY = maze[destination.x][destination.y].yCenter;

  if(destination.x == 0 && destination.y == -1)
  {
    targetX = startingSectorX;
    targetY = startingSectorY;
  }

	rotateTowardsTarget(targetX, targetY, phase);
  if(IR_ADJUSTMENT)
  {
    if(phase == 1 && destination.y != -1)
    {
      IR_targetAdjustment(destination);
      targetX = maze[destination.x][destination.y].xCenter;
      targetY = maze[destination.x][destination.y].yCenter;
    }
  }
  moveToTarget(targetX, targetY, phase);
  if(US_ADJUSTMENT)
  {
    if(phase != 2 && destination.y != -1)
      US_targetAdjustment(destination);
  }
}

void centerStartingPosition()
{
  adjustAngle();
  US_targetAdjustment((XY){.x = 5,.y = 5});
  usleep(SLEEPTIME);
  turnByAngleDegree(90.00);
  usleep(SLEEPTIME);
  adjustAngle();
  usleep(SLEEPTIME);
  US_targetAdjustment((XY){.x = 5,.y = 5});
  usleep(SLEEPTIME);
  turnByAngleDegree(90.00);
  usleep(SLEEPTIME);
}

void endPhase1()
{
  int usValue = get_us_dist();
  while(usValue > DETECT_WALL_DISTANCE_U)
  {
    set_motors(2,2);
    usValue = get_us_dist();
  }
  set_motors(0,0);
  centerStartingPosition();
  updateRobotPosition();
	set_ir_angle(LEFT, 90);
	set_ir_angle(RIGHT, -90);
	sleep(1);
  set_ir_angle(LEFT, 45);
  set_ir_angle(RIGHT, -45);
  sleep(1);
}
