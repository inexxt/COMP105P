#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include "static_functions.h"
#include <math.h>
#define SLEEPTIME 1000
#define BS 10

double k = 0;
double Kp = 0.015;
double Kd = 0.001;
double normal_dist = 30;

double diff = 1, pdiff = 1, lspeed = BS, rspeed = BS;

int main()
{
	connect_to_robot();
	initialize_robot();
	while(1)
	{
		usleep(SLEEPTIME);
		
		diff = normal_dist - leftFsensor(); //if more than 0, we're too close
		
		k = diff/normal_dist;
		
		lspeed += k*(Kp*fabs(diff) - Kd*(pdiff - diff)/SLEEPTIME);
		rspeed -= k*(Kp*fabs(diff) - Kd*(pdiff - diff)/SLEEPTIME);
		
		
// 		lacc = diff*sqrt(fabs(diff))*0.0001;
// 		racc = -diff*sqrt(fabs(diff))*0.0001;
// 		
// 		lspeed += lacc;
// 		rspeed += racc;
// 		
// 		lspeed += lacc*0.001*lspeed;
// 		rspeed += racc*0.001*rspeed;
// 		
// 		lspeed -= diff*0.004;
// 		rspeed -= diff*0.004;
		
	
		
		if(fabs(rspeed) > 255) rspeed = (rspeed)/fabs(rspeed)*255;
		if(fabs(lspeed) > 255) lspeed = (lspeed)/fabs(lspeed)*255;
		
		
		
		printf("LS %.2f\tRS %.2f\t%.2f DERIV %.2f\n", lspeed, rspeed, diff, k, k*diff*Kp, Kd*(pdiff - diff)/SLEEPTIME);
		printf("FUEL %.2f\n", fuel);
		set_motors((int)lspeed, (int)rspeed);
		pdiff = diff;
	}
}
