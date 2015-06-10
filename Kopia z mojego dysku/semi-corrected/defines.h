#include <unistd.h>
#include <stdlib.h>
#include "picomms.h"
#include <stdio.h>
#include <math.h>

#define SIMULATOR 1
// #define REAL_ROBOT 1

#ifdef SIMULATOR
#define ROBOT_WIDTH 22.5
#define CIRCUMFERENCE 9.50*M_PI
#define US_OFFSET 2 //difference between av. IR and US values (simulator - 2, real robot ~ 6.5)
#define SECTOR_WIDTH 60
#endif

#ifdef REAL_ROBOT
#define ROBOT_WIDTH 23.65
#define CIRCUMFERENCE 9.95*M_PI
#define US_OFFSET 6.5 //difference between av. IR and US values(simulator - 2, real robot ~ 6.5)
#define SECTOR_WIDTH 59
#endif

/*
calibration values:
Hugo: 22.95, 9.60
Klaus: 23.90, 10.00
Gus: 23.90, 10.00
Fifi: 23.90, 10.00
Rita: 23.90, 10.00
Igor: 23.90, 10.00
Jeanne: 23.85, 10.00
*/

#define MEDIUM_SPEED 32

#define MAZE_WIDTH 4
#define MAZE_HEIGHT 4
#define NUMBER_OF_IR_READINGS 8
#define NUMBER_OF_US_READINGS 4

#define SLEEPTIME 300000

#define DETECT_WALL_DISTANCE 38
#define DETECT_WALL_DISTANCE_U 40

extern double xPos;
extern double yPos;
extern double bearing;

extern double targetWallReadings;
extern double sensorDifference; // difference between average of front and side sensors (if pointed in the same direction)

extern int leftEncoder; // values to be kept and used by the recordPosition 
extern int rightEncoder; 