//Writen by Stuczynski, Jedrzej Jakub and Hu, Zheng He
//This program makes robot follow a wall until it detects a corner. 
//Then the robot stops and goes back to the starting position without any sensors.

#include <unistd.h>
#include <stdlib.h>
#include "picomms.h"
#include "basicMovement.h"
#include "recordPosition.h"
#include <stdio.h>
#include <math.h>

#define CORNER_DISTANCE 23
#define DISTANCE_TO_KEEP_SIDE 24
#define DISTANCE_TO_KEEP_FRONT 33
#define MEDIUM_SPEED 50
#define DISTANCE_TO_STOP_AT 16
#define MINIMUM_DISTANCE_BETWEEN_POINTS 300

int frontLeftIR = 0;
int frontRightIR = 0;
int sideLeftIR = 0;

int sideRightIR = 0;
int usValue = 0;
double bearing = 0.0000;
double xPos = 0.0000;
double yPos = 0.0000;
double xStorage[2000];
double yStorage[2000];
int size = 1;

void updateIRSensors()
{
  get_front_ir_dists(&frontLeftIR, &frontRightIR);
  get_side_ir_dists(&sideLeftIR, &sideRightIR);
}

void slowDown()
{
  int offSet = 0;
  while(usValue > DISTANCE_TO_STOP_AT)
  {
    usValue = get_us_dist();
    offSet = usValue - DISTANCE_TO_STOP_AT;
    set_motors(offSet*2, offSet*2);
  }
  set_motors(0,0);
}

void showCoordinates()
{
  int i = 0;
  for(i = 0; i <= size; i++)
  {
    printf("X: %f \t Y: %f \n",*(xStorage+i), *(yStorage+i));
    set_point(*(xStorage+i)/10.00,*(yStorage+i)/10.00);  
  }
}

void storeCoordinates(int *storageSize)
{
	/*
	if(*storageSize < size)
  	{  
    	*storageSize *= 2;
    	xStorage = (double *) realloc (xStorage, (sizeof(double) * (*storageSize)));
    	yStorage = (double *) realloc (yStorage, (sizeof(double) * (*storageSize)));
  	}*/
  	*(xStorage + size) = xPos;
    *(yStorage + size) = yPos;
}

void followLeft()
{ 
  int leftSpeed, rightSpeed;
  int differenceSide, differenceFront;
  double theConstantLeft, theConstantRight;
  int storageSize = 1;
  while(1)
  {
    updateRobotPosition(&bearing, &xPos, &yPos);
    storeCoordinates(&storageSize);
    
    updateIRSensors();
    differenceSide = DISTANCE_TO_KEEP_SIDE - sideLeftIR;
    differenceFront = frontLeftIR - DISTANCE_TO_KEEP_FRONT;
   
    theConstantLeft = 0.40; 
    theConstantRight = 0.85;

    if(differenceSide < -30)
    {
      differenceSide = -30;
      theConstantLeft = 0.10; 
    }
    if(differenceFront > 30)
    {
      differenceFront = 30;
      theConstantRight = 0.90;
    }

    if(differenceFront < -8)
    {
      theConstantRight = 1.6;
      theConstantLeft = 1.2;
    }

    leftSpeed = MEDIUM_SPEED + MEDIUM_SPEED * ((double)differenceSide/DISTANCE_TO_KEEP_SIDE*theConstantLeft);
    rightSpeed = MEDIUM_SPEED + MEDIUM_SPEED * ((double)differenceFront/DISTANCE_TO_KEEP_FRONT*theConstantRight);
 
    set_motors(leftSpeed,rightSpeed);

    if(frontRightIR < CORNER_DISTANCE)
    {
      set_motors(10,10);
      usValue = get_us_dist();
      if(usValue < DISTANCE_TO_STOP_AT+2)
      {
        slowDown();
        set_motors(0,0);
        break;
      }
    } 
    size++;
  }
 }

void goBack()
{
  double xDifference;
  double yDifference;
  double requiredAngleChange;
  double remainingDistance;
  int start = 1;
  size -= 40;
  while (size > 0) // while there are still stored points left
  {
    log_trail();
    updateRobotPosition(&bearing, &xPos, &yPos);
  	xDifference = *(xStorage+size) - xPos;
    yDifference = *(yStorage+size) - yPos;
    requiredAngleChange = atan2(xDifference,yDifference) - bearing;
    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);

    /*So the robot would operate in a -pi to +pi range: */
  	if(requiredAngleChange > (M_PI))
  	  requiredAngleChange -= (2*M_PI);
  	if(requiredAngleChange < (-M_PI))
  	  requiredAngleChange += (2*M_PI);

    if((fabs(xDifference) < 5.0) && (!start)) // start ensures the robot will rotate at the beginning regardless of relative position
    {
    	if(yDifference > 0)
    		requiredAngleChange = 0;
    	else
    		requiredAngleChange = M_PI;
    	start = 0;
    }

  	/**********DEBUG**********/
    printf("\nCurrently at... %f     %f\n",xPos,yPos);
    printf("Bearing: %f\n", bearing);
    printf("going to... %f     %f\n", (*(xStorage+size)),(*(yStorage+size)));
    set_point(*(xStorage+size)/10.00,*(yStorage+size)/10.00);  // draw the point the robot is going to
    printf("Remaining distance: %f\n", remainingDistance);
    printf("xDif: %f\tyDif: %f\n", xDifference, yDifference);
    printf("\nRequired angle change: %f\n", requiredAngleChange);
    /**********DEBUG**********/
     
    if(remainingDistance > 1000) // breaks the loop in case the robot goes wild
      return;

    if(remainingDistance < 70)
    {
    	double distanceFromNextPoint = 0.0;
    	while(distanceFromNextPoint < MINIMUM_DISTANCE_BETWEEN_POINTS)
    	{
    	  xDifference = *(xStorage+size) - xPos;
 		  yDifference = *(yStorage+size) - yPos;
 		  distanceFromNextPoint = sqrt(xDifference*xDifference + yDifference*yDifference);
 		  size--;
 		  if(size == 1) //ensures robot will go to the starting position
    	  	break;
    	}
    }

    if(fabs(requiredAngleChange) > 1.8) // for sharp turns, in practice only for turning back at the beginning
    {
    	while(fabs(requiredAngleChange) > 0.30)
    	{
    	  log_trail();
          updateRobotPosition(&bearing, &xPos, &yPos);
          requiredAngleChange = atan2(xDifference,yDifference) - bearing;
          set_motors(requiredAngleChange*14.0,-requiredAngleChange*14.0);
    	}
    }
    else
    {
      log_trail();
      updateRobotPosition(&bearing, &xPos, &yPos);
      set_motors((MEDIUM_SPEED + MEDIUM_SPEED * requiredAngleChange*1.2),(MEDIUM_SPEED - MEDIUM_SPEED * requiredAngleChange*1.2));
    }
  }
}

int main() 
{
  connect_to_robot();
  initialize_robot();
  set_origin();

  set_ir_angle(LEFT, 0);
  set_ir_angle(RIGHT, -75);   
  //xStorage = malloc(sizeof(double) * 1);
  //yStorage = malloc(sizeof(double) * 1);
  *xStorage = 0.0000;
  *yStorage = 0.0000;
  
  followLeft();

  showCoordinates();
  printf("Final coordinates: X: %f \t Y: %f\n",xPos,yPos);
  printf("Distance from origin: %f mm\n", (sqrt(xPos*xPos + yPos * yPos)));
  printf("at angle: %f (radians) or %f (degrees) \n", bearing, (toDegree(bearing)));

  set_motors(0,0);
  printf("Trying to go back...\n");
  goBack();
  set_motors(0,0);
  //free(xStorage);
  //free(yStorage);
  return 0;
}