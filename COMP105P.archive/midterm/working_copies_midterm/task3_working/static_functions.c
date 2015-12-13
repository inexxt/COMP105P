#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include <math.h>
#include "static_functions.h"
#include "defines.h"

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

int rightFsensor()
{
	get_front_ir_dists(&left, &right);
	return right;
}

int leftSsensor()
{
	get_side_ir_dists(&left, &right);
	return left;
}

int rightSsensor()
{
	get_side_ir_dists(&left, &right);
	return right;
}

int sign(int x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}

double thirdSideOfTriangle(double a, double b, double alpha)
{
	return fabs(1/2*a*b*sin(alpha));
}

double secondAngle(double a, double alpha, double b)
{
	return asin(b*sin(alpha)/a);
}

double computeAngle(double dl, double dr)
{
	double R = R_WIDTH;
	return (dl-dr)/R;
}

double computeWay(double dl, double dr, double alpha, int which)
{
	double way = 0;
	
	if(alpha != 0 && alpha != -0)
	{
		double r_r = dr/alpha;
		double r_l = dl/alpha;
		double rm = (r_l+r_r)/2;
		way = sqrt(2*rm*rm- 2*rm*rm*cos(alpha));
		printf("r %f\tway %f\talpha %f\tdl %f\tdr %f\n", rm, way, alpha, dl, dr);
	}
	else
	{
		way = dr;
	}
	return way;
}

