#include <unistd.h>
#include <stdlib.h>
#include "picomms.h"
#include <stdio.h>
#include <math.h>



#define ROBOT_WIDTH 22.5//23.4//22.0//23.0//22.50 
#define CIRCUMFERENCE 9.5*M_PI//9.8*M_PI//9.9*M_PI//9.2*M_PI//9.8*M_PI//9.5*M_PI

#define SECTOR_WIDTH 59.0//59.0
#define MEDIUM_SPEED 22

#define MAZE_WIDTH 4
#define MAZE_HEIGHT 4
#define NUMBER_OF_IR_READINGS 8
#define NUMBER_OF_US_READINGS 4

#define SLEEPTIME 150000

#define DETECT_WALL_DISTANCE 38
#define DETECT_WALL_DISTANCE_U 40

extern double xPos;
extern double yPos;
extern double bearing;

extern double targetWallReadings;
extern double sensorDifference;

extern int leftEncoder;
extern int rightEncoder;