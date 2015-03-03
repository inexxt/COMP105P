
#define ROBOT_WIDTH 225.000  // real robot: 235
#define CIRCUMFERENCE 95*M_PI // real robot: 100

#include <unistd.h>
#include <stdlib.h>
#include "picomms.h"
#include <stdio.h>
#include <math.h>

int leftEncoder,rightEncoder;
double previousLeftEncoder = 0.000;
double previousRightEncoder = 0.000;
double previousTheta = 0.000;
double deltaX = 0.0000;
double deltaY = 0.0000;
double theta = 0.0000;

void updateEncoders()
{
  get_motor_encoders(&leftEncoder, &rightEncoder);
}

double toDegree(double radians)
{
  double inDegree = radians*180.00/M_PI;
  if(inDegree < 0)
    return (360.00 + inDegree);
  else
    return inDegree; 
}

void updateAngle(double distanceLeft, double distanceRight)
{
  double angleChange = ((distanceLeft - distanceRight) / ROBOT_WIDTH);
  theta += angleChange;
}

void updateX(double distanceLeft, double distanceRight)

{
   deltaX = deltaX + ( (distanceRight + distanceLeft) / (2) ) * sin(theta);
 }

void updateY(double distanceLeft, double distanceRight)
{
  deltaY = deltaY + ( (distanceRight + distanceLeft) / (2) ) * cos(theta);
}

void updateRobotPosition(double *angle, double *xPos, double *yPos)
{
  *xPos = deltaX;
  *yPos = deltaY;
  *angle = theta;
  
  previousLeftEncoder = leftEncoder;
  previousRightEncoder = rightEncoder;

  updateEncoders();

  double leftOffset = (double)leftEncoder - previousLeftEncoder;
  double rightOffset = (double)rightEncoder - previousRightEncoder;
   
  double distanceLeft = leftOffset * (CIRCUMFERENCE/360);
  double distanceRight = rightOffset * (CIRCUMFERENCE/360);

  previousTheta = theta;  
  updateAngle(distanceLeft, distanceRight);
  updateX(distanceLeft,distanceRight);
  updateY(distanceLeft,distanceRight);
}
