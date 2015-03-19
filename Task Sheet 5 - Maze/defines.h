#include <unistd.h>
#include <stdlib.h>
#include "picomms.h"
#include <stdio.h>
#include <math.h>



#define ROBOT_WIDTH 22.50 
#define CIRCUMFERENCE 9.5*M_PI
#define SECTOR_WIDTH 60.00
#define MEDIUM_SPEED 32
#define MAZE_WIDTH 4
#define MAZE_HEIGHT 4

extern double xPos;
extern double yPos;
extern double bearing;