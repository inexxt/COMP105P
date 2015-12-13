#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include <math.h>
#include "static_functions.h"
#define D360 1697 //defining one full circle turn in terms of robot's wheels' degrees
#define M1 754 //defining one meter lenght in terms of robot's wheels' degrees
#define SLEEPTIME 1000
#define MAXACC_S 0.003 //maximal straight line acceleration/decceleration while wheels still do not slide / 10e-3second
#define MAXACC_T 0.001 //maximal turning acceleration/decceleration while wheels still do not slide / 10e-3second

int left, right, var;

int leftencoder()
{
	get_motor_encoders(&left, &right);
	return left;
}

int rightencoder()
{
	get_motor_encoders(&left, &right);
	return right;
}

void turn(int degrees)
{
	int Linv = abs(leftencoder());
	int Rinv = abs(rightencoder()); 
	int currentvalue = 0;
	int target = D360*degrees/360;
	double acc = MAXACC_T * SLEEPTIME;
	double dspeed = 0;
	int speed = 0;
	
	while (currentvalue < target)
	{
		speed = dspeed;
		
		set_motors(speed, -speed); //
		usleep(SLEEPTIME);
		
		currentvalue=abs(leftencoder() - Linv) + abs(rightencoder() - Rinv);
		dspeed = (currentvalue < target/2) ? (dspeed + acc) : (dspeed - acc);
		
		log_trail();
		printf("L %d R %d C %d\n", leftencoder(), rightencoder(), currentvalue);
	}
	set_motors(0, 0);
	sleep(1); //TODO maybe remove it later
}

void straight(double length)
{
	int initialvalue = leftencoder();
	int currentvalue = 0;
	int target = M1*length;
	double acc = MAXACC_S * SLEEPTIME;
	double dspeed = 0;
	int speed = 0;
	
	while (currentvalue - initialvalue < target)
	{
		speed = dspeed;
		
		set_motors(speed, speed);
		usleep(SLEEPTIME);
		
		dspeed = (currentvalue - initialvalue < target/2) ? (dspeed + acc) : (dspeed - acc);
		currentvalue=leftencoder();
		
		log_trail();
		printf("R C %d I %d\n", currentvalue, initialvalue);
	}
	
	set_motors(0,0);
	sleep(1); //TODO maybe remove it later
}

int leftFsensor()
{
	get_front_ir_dists(&left, &right);
	return left;
}
