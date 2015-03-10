#include "defines.h"

int leftEncoder,rightEncoder;
double previousLeftEncoder = 0.0000;
double previousRightEncoder = 0.0000;
double xPos, yPos, bearing; // global variables from extern in defines.h

void updateEncoders()
{
  get_motor_encoders(&leftEncoder, &rightEncoder);
}

void updateAngle(double distanceLeft, double distanceRight)
{
  double angleChange = ((distanceLeft - distanceRight) / ROBOT_WIDTH);
  bearing += angleChange;
  if(bearing > (2 * M_PI))
    bearing -= (2 * M_PI);
  if(bearing < 0)
    bearing += (2 * M_PI);
}

void updateX(double distanceLeft, double distanceRight)

{
   xPos += ( (distanceRight + distanceLeft) / (2) ) * sin(bearing);
 }

void updateY(double distanceLeft, double distanceRight)
{
  yPos += ( (distanceRight + distanceLeft) / (2) ) * cos(bearing);
}

void updateRobotPosition()
{
  previousLeftEncoder = leftEncoder;
  previousRightEncoder = rightEncoder;
  updateEncoders();

  double leftOffset = (double)leftEncoder - previousLeftEncoder;
  double rightOffset = (double)rightEncoder - previousRightEncoder;
  double distanceLeft = leftOffset * (CIRCUMFERENCE/360);
  double distanceRight = rightOffset * (CIRCUMFERENCE/360);

  updateAngle(distanceLeft, distanceRight);
  updateX(distanceLeft,distanceRight);
  updateY(distanceLeft,distanceRight);
}
