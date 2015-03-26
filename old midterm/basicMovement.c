#include "picomms.h"
#include "defines.h"

#define ENCODER_TICKS_PER_MM 1.322

int speedBig = 15;
int speedSmall = 2;
int leftEncoder, rightEncoder;

void turnRight()
{
  set_motors(0,0);
  int targetEncoder = 212; // to update
  get_motor_encoders(&leftEncoder, &rightEncoder);
  int previousLeftEncoder = leftEncoder;
  while((leftEncoder - previousLeftEncoder) < (targetEncoder - 15))
  {
  	set_motors(speedBig,-speedBig);
    get_motor_encoders(&leftEncoder, &rightEncoder);
  }
  while((leftEncoder - previousLeftEncoder) < targetEncoder)
  {
  	set_motors(speedSmall,-speedSmall);
    get_motor_encoders(&leftEncoder, &rightEncoder);
  }
}

void turnLeft()
{
  set_motors(0,0);
  int targetEncoder = 214; // to update
  get_motor_encoders(&leftEncoder, &rightEncoder);
  int previousRightEncoder = rightEncoder;
  while((rightEncoder - previousRightEncoder) < (targetEncoder - 15))
  {
  	set_motors(-speedBig,speedBig);
    get_motor_encoders(&leftEncoder, &rightEncoder);
  }
  while((rightEncoder - previousRightEncoder) < targetEncoder)
  {
  	set_motors(-speedSmall,speedSmall);
    get_motor_encoders(&leftEncoder, &rightEncoder);
  }
}

void moveForward(int distance, int speed)
{
  get_motor_encoders(&leftEncoder, &rightEncoder);
  int previousLeftEncoder = leftEncoder;
  int targetEncoder = ENCODER_TICKS_PER_MM * distance;
  while((leftEncoder - previousLeftEncoder) < (targetEncoder - 15))
  {
  	set_motors(speed,speed);
    get_motor_encoders(&leftEncoder, &rightEncoder);
  }
  while((leftEncoder - previousLeftEncoder) < targetEncoder)
  {
  	set_motors(speedSmall,speedSmall);
    get_motor_encoders(&leftEncoder, &rightEncoder);
  }
  set_motors(0,0);
}

void turnByAngleDegree(double angle)
{
  int leftEncoder, rightEncoder;
  get_motor_encoders(&leftEncoder, &rightEncoder);
  double spinCircumference = ROBOT_WIDTH * M_PI;
  double distanceToCover = spinCircumference * (angle/360.00);
  double tickDifference = (360*distanceToCover)/(CIRCUMFERENCE);
  int targetLeftEncoder = leftEncoder + tickDifference;
  int speed;
  while(abs(targetLeftEncoder - leftEncoder) > 1)
  {
    speed = (targetLeftEncoder - leftEncoder);
    if(speed > 20)
      speed = 20;
    if(speed < -20)
      speed = -20;
//     printf("speed: %d\n",speed);
    get_motor_encoders(&leftEncoder, &rightEncoder);
    set_motors(speed, -speed);
  }
  set_motors(0,0);
}