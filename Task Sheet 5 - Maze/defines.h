#include <unistd.h>
#include <stdlib.h>
#include "picomms.h"
#include <stdio.h>
#include <math.h>



#define ROBOT_WIDTH 225.000 
#define CIRCUMFERENCE 95*M_PI
#define SECTOR_WIDTH 600.00
#define MEDIUM_SPEED 32

double xPos;
double yPos;
double bearing;