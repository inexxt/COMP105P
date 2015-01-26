#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include <math.h>
#include "static_functions.h"
#define D360 1697 //defining one full circle turn in terms of robot's wheels' degrees
#define M1 754 //defining one meter lenght in terms of robot's wheels' degrees
#define SLEEPTIME 10000


int left, right;

int leftencoder(){
	get_motor_encoders(&left, &right);
	return left;
}

int rightencoder(){
	get_motor_encoders(&left, &right);
	return right;
}

void turn(int degrees){
	int Linv = abs(leftencoder());
	int Rinv = abs(rightencoder()); 
	int currentvalue = 0;
	while (currentvalue < D360*degrees/360)
	{
		set_motors(5, -5);
		usleep(SLEEPTIME);
		log_trail();
		currentvalue=abs(leftencoder() - Linv) + abs(rightencoder() - Rinv);
		printf("L %d R %d C %d\n", leftencoder(), rightencoder(), currentvalue);
	}
	set_motors(0, 0);
	sleep(1);
}

void straight(double length){
	int initialvalue = leftencoder();
	int currentvalue = 0;
	int target = M1*length;
	while (currentvalue - initialvalue < target)
	{
		set_motors(speed, speed);
		usleep(SLEEPTIME);
		log_trail();
		currentvalue=leftencoder();
		printf("R C %d I %d\n", currentvalue, initialvalue);
	}
	set_motors(0,0);
	sleep(1);
}

