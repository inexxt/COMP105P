#include "picomms.h"
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