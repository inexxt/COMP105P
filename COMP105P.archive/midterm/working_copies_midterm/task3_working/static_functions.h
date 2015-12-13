#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include <math.h>

void turn(int degrees);
void straight(double length);
double thirdSideOfTriangle(double a, double b, double alpha);
double secondAngle(double a, double alpha, double b);
double computeAngle(double dl, double dr);
double computeWay(double dl, double dr, double alpha, int which);
