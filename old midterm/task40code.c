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
#include "phase1.move.h"

#define CORNER_DISTANCE 23
#define DISTANCE_TO_KEEP_SIDE 20
#define DISTANCE_TO_KEEP_FRONT 25

#define DISTANCE_TO_STOP_AT 24
#define MINIMUM_DISTANCE_BETWEEN_POINTS 300

#define PHA1 1
#define PHA2 2


int frontLeftIR = 0;
int frontRightIR = 0;
int sideLeftIR = 0;

int sideRightIR = 0;
int usValue = 0;
double xStorage[20000];
double yStorage[20000];
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

void goBackALittle()
{
// 	set_motors(-3, -3);
// 	usleep(500000);
// 	set_motors(0,0);
}

void followLeft(int type)
{ 
  int leftSpeed, rightSpeed;
  int differenceSide, differenceFront;
  double theConstantLeft, theConstantRight;
  int storageSize = 1;
  int vaar = 1;
  int ending = 0;
  while(ending == 0)
  {
    updateRobotPosition();
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
    if(differenceFront > 10)
    {
//       differenceFront = 15;
//       theConstantRight = 0.90;
		vaar = 0;
    }

    if(differenceFront < -8)
    {
      theConstantRight = 1.6;
      theConstantLeft = 1.2;
    }
   
	if(vaar)
	{
		leftSpeed = MEDIUM_SPEED + MEDIUM_SPEED * ((double)differenceSide/DISTANCE_TO_KEEP_SIDE*theConstantLeft);
		rightSpeed = MEDIUM_SPEED + MEDIUM_SPEED * ((double)differenceFront/DISTANCE_TO_KEEP_FRONT*theConstantRight);
	}
	else
	{
		if(differenceSide < -10)
			leftSpeed = 5;
		else
			leftSpeed = 18	;
		rightSpeed = 30;
		vaar = 1;
	}
	printf("%d %d %.2f %.2f\n", leftSpeed, rightSpeed, xPos, yPos);
    set_motors(leftSpeed,rightSpeed);

    if(frontRightIR < CORNER_DISTANCE)
    {
      set_motors(10,10);
      usValue = get_us_dist();
      if(usValue < DISTANCE_TO_STOP_AT+2)
      {
//         slowDown();
        set_motors(0,0);
		goBackALittle();
		turnByAngleDegree(90);
//         turnRight();
// 		bearing += M_PI/2;
      }
      
    }
    int EPS = 10;
      if(size > 100 && (fabs(xPos) < EPS && fabs(yPos + 60) < EPS) && type == PHA1)
	  {
		  ending = 1;
	  }
	  if(size > 100 && (fabs(xPos - 180) < EPS && fabs(yPos - 180) < EPS) && type == PHA2)
	  {
		  ending = 1;
	  }
//     if(size > 100 && xPos < 
    size++;
  }
  set_motors(0,0);
}

int main() 
{
  connect_to_robot();
  initialize_robot();
  set_origin();
  
  centerStartingPosition();
  set_ir_angle(LEFT, -16);
  set_ir_angle(RIGHT, -75);   
  //xStorage = malloc(sizeof(double) * 1);
  //yStorage = malloc(sizeof(double) * 1);
  *xStorage = 0.0000;
  *yStorage = 0.0000;
  
  followLeft(PHA1);
  printf("bearing %.2f %.2f\n", bearing, -bearing/M_PI * 180);
  int p;
  scanf("%d", &p);
  turnByAngleDegree(-bearing/M_PI * 180 - 20);
  usleep(500000);
  centerStartingPosition();
  bearing = 0;
  xPos = 0;
  yPos = -SECTOR_WIDTH;
  set_ir_angle(LEFT, -16);
  set_ir_angle(RIGHT, -75);   
  
  followLeft(PHA2);
//   showCoordinates();
//   printf("Final coordinates: X: %f \t Y: %f\n",xPos,yPos);
//   printf("Distance from origin: %f mm\n", (sqrt(xPos*xPos + yPos * yPos)));
// //   printf("at angle: %f (radians) or %f (degrees) \n", bearing, (toDegree(bearing)));
  
  
  set_motors(0,0);
  printf("Trying to go back...\n");
//   goBack();
  set_motors(0,0);
  //free(xStorage);
  //free(yStorage);
  return 0;
}