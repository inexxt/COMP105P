#include <unistd.h>
#include <stdlib.h>
#include "picomms.h"
#include <stdio.h>
#include <math.h>



#define ROBOT_WIDTH 22.5//22.0//23.0//22.50 
#define CIRCUMFERENCE 9.5*M_PI//9.2*M_PI//9.8*M_PI//9.5*M_PI
#define SECTOR_WIDTH 60.0//58.0//60.00//64.00//60.00
#define MEDIUM_SPEED 32
#define MAZE_WIDTH 4
#define MAZE_HEIGHT 4

extern double xPos;
extern double yPos;
extern double bearing;