#include "phase1.h"

#define MAPP 1
#define CORR 2

XY getCurrentSector()
{
	int xSquare = round(xPos/SECTOR_WIDTH);
	int ySquare = round(yPos/SECTOR_WIDTH);

	return (XY){.x = xSquare, .y = ySquare};
}

void printCurrentSector()
{
	XY cs = getCurrentSector();
	printf("DEBUG CUR\tX %d Y %d\n", cs.x, cs.y);
}

void getSideIR(double *left, double *right, int option)
{
    if(option == CORR)
    {
        int i;
        for(i = 0; i < NUMBER_OF_IR_READINGS; i++)
        {
            int leftReading, rightReading;
            get_side_ir_dists(&leftReading, &rightReading);
            usleep(5000);
            *left += leftReading;
            *right += rightReading;
        }
        *left /= NUMBER_OF_IR_READINGS*1.0;
        *right /= NUMBER_OF_IR_READINGS*1.0;
    }
    if(option == MAPP)
    {
        int i, minl=1000, minr=1000, maxl=00, maxr = 00;
        for(i = 0; i < NUMBER_OF_IR_READINGS; i++)
        {
            int leftReading, rightReading;
            get_side_ir_dists(&leftReading, &rightReading);
            usleep(5000);
            if(leftReading < minl) minl = leftReading;
            if(leftReading > maxl) maxl = leftReading;
            if(rightReading < minr) minr = rightReading;
            if(rightReading > maxr) maxr = rightReading;
        }
        *left /= NUMBER_OF_IR_READINGS*1.0;
        *right /= NUMBER_OF_IR_READINGS*1.0;
    }
        
}

void getFrontIR(double *left, double *right)
{
  int i;
  for(i = 0; i < NUMBER_OF_IR_READINGS; i++)
  {
  	int leftReading, rightReading;
  	get_front_ir_dists(&leftReading, &rightReading);
    usleep(5000);
  	*left += leftReading;
  	*right += rightReading;
  }
  *left /= NUMBER_OF_IR_READINGS*1.0;
  *right /= NUMBER_OF_IR_READINGS*1.0;
}

double getUSValue()
{
  double USValue;
  int i;
  for(i = 0; i < NUMBER_OF_US_READINGS; i++)
  {
    USValue += get_us_dist();
    usleep(5000);

  }
  USValue /= NUMBER_OF_US_READINGS*1.0;
  return USValue;
}