#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include "static_functions.h"
#include <math.h>
#define BS 20
#define ST 1000
#define R_WIDTH 17
#define FDIST 30
#define SDIST 10

int sign(int x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}


float fuel = 2.5*BS;

float err=0, diff = 0, i=0, lspeed = BS, rspeed = BS, pdiff = 0, perr, Ke = 0.1, acc = 0, Kd = 0.3, p, y, i;

float pi = 3.141;

float rangle = 60, langle = 30; 

void fuelcheck()
{
	if(fuel < fabs(lspeed) + fabs(rspeed))
	{
		lspeed = fuel * lspeed/(fabs(lspeed) + fabs(rspeed));
		rspeed = fuel * rspeed/(fabs(lspeed) + fabs(rspeed));
		fuel = 0;
		printf("\tOUT OF FUEL!\n");
	}
	else
	{
		fuel -= fabs(lspeed) + fabs(rspeed);
	}
	fuel += 2*BS;
}

void update_angles()
{
	i++;
	set_ir_angle(RIGHT, -135+rangle);
	set_ir_angle(LEFT, -45+langle);
	sleep(1);

	rangle = rangle*2*pi/360.0;
	langle = langle*2*pi/360.0;
}

int main()
{
	connect_to_robot();
	initialize_robot();
	
	set_ir_angle(RIGHT, -135+rangle);
	set_ir_angle(LEFT, -45+langle);
	sleep(1);

	rangle = rangle*2*pi/360.0;
	langle = langle*2*pi/360.0;	
	while(1)
	{
		lspeed = BS;
		rspeed = BS;
		
		diff = FDIST - leftFsensor(); //if positive turn left 
// 		crit = SDIST - leftSsensor();
		
		y = (leftFsensor()*cos(langle) + R_WIDTH)/cos(langle);
		p = y*cos(langle)/cos(rangle);
		
		err = rightFsensor() - p; //if positive turn left
			
		printf("RCOS %.2f\tLCOS %.2f R %.2f L%.2f\n", cos(rangle), cos(langle), rangle, langle);
		printf("Y %.2f\tP %.2f ERR %.2f\n", y, p, err);
		
// 		err = R_WIDTH - (rightFsensor()*cos(rangle) - leftFsensor()*cos(langle)); //if positive, turn left
		
		acc -= Ke*(err - 0*fabs(err - perr));
		
// 		int x = 20;
// 		acc += Kd*sign(diff)*exp((fabs(diff)-x) - 0.5*fabs(diff - pdiff));
		
		acc += Kd*(diff*fabs(diff) - 0.5*fabs(diff - pdiff));
		lspeed += acc;
		rspeed -= acc;
		
		log_trail();
		fuelcheck();
		update_angles();
		
		//printf("LS %.2f\tRS %.2f\tER %.2f\tDF %.2f\tEXP %.2f\n", lspeed, rspeed, err, diff, Kd*(diff - 0.2*fabs(diff - pdiff))); 
		//rspeed += err*0.001;
		set_motors((int)lspeed, (int)rspeed);
		usleep(ST);
		pdiff = diff;
		perr = err;
		acc = 0;
	}

}
