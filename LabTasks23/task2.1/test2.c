#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include "static_functions.h"
#include <math.h>
#define BS 40 		// basic speed
#define ST 100 	//sleep time
#define R_WIDTH 17 	//robot R_WIDTH
#define OD 20 		//optimal distance

int sign(int x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}


float fuel = 2.2*BS;

float err=0, l_diff = 0, r_diff = 0, lspeed = BS, rspeed = BS, pl_diff = 0, pr_diff = 0, perr = 0, Ke = 0.4, Ke_d = 0.2, acc = 0, Kd = 0.1, p, y;

float Ks = 0.8, Ks_d = 0.5;

float l_opt_dist = 0, r_opt_dist = 0;

int i = 0, sig = 1;

float pi = 3.141;

float start_rangle = 50, start_langle = 30; 

float rangle = 0, langle = 0; 

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

void degrees_to_radians()
{
	rangle = rangle*2*pi/360.0;
	langle = langle*2*pi/360.0;
}

void update_angles()
{
	i += sig;
	
	if(i%14 == 0)
	{
		sig = -sig;
	}
	langle = start_langle - 2*i;
	rangle = start_rangle + i;
	
	printf("\t\tL %.2f\tR %.2f\ti %d\n", langle, rangle, i);
	
	set_ir_angle(RIGHT, (int)(-135+rangle));
	set_ir_angle(LEFT, (int)(-45+langle));
	
	degrees_to_radians();
}

int main()
{
	connect_to_robot();
	initialize_robot();
	
	set_ir_angle(RIGHT, -135+start_rangle);
	set_ir_angle(LEFT, -45+start_langle);
	
	sleep(1);
	
	degrees_to_radians();
	
	while(1)
	{
		l_opt_dist = OD/cos(langle);
		r_opt_dist = (OD+R_WIDTH)/cos(rangle);
		
		lspeed = BS;
		rspeed = BS;
		
		l_diff = l_opt_dist - leftFsensor(); //if positive turn left 
		r_diff = r_opt_dist - rightFsensor(); //if positive turn right (fast escape!)
		
		y = (leftFsensor()*cos(langle) + R_WIDTH)/cos(langle);
		p = y*cos(langle)/cos(rangle);		
		err = rightFsensor() - p; //if positive turn left
			
		printf("RCOS %.2f\tLCOS %.2f R %.2f L%.2f\n", cos(rangle), cos(langle), rangle, langle);
		printf("Y %.2f\tP %.2f ERR %.2f\n", y, p, err);
		
		
		acc -= Ke*(err - Ke_d*fabs(err - perr));
		acc += Kd*(l_diff*fabs(l_diff) - 0.2*fabs(l_diff - pl_diff));
		
		
//		printf("\t\t\t\t\t %d\n", rightFsensor());
// 		if(rightFsensor() < 30)
// 		{
// 			printf("\t\t\t\t%.2f %.2f %.2f\n", Ks*exp(r_diff-30 - Ks_d*fabs(l_diff - pl_diff)), r_diff-30, Ks_d*fabs(l_diff - pl_diff));
// 			//scanf("%f", &p);
// 		}
		
		acc += Ks*exp(r_diff-30 - Ks_d*fabs(l_diff - pl_diff));
		
		lspeed += acc;
		rspeed -= acc;
		
		log_trail();
		fuelcheck();
		update_angles();
		
		//printf("LS %.2f\tRS %.2f\tER %.2f\tDF %.2f\tEXP %.2f\n", lspeed, rspeed, err, l_diff, Kd*(l_diff - 0.2*fabs(l_diff - pl_diff))); 

		set_motors((int)lspeed, (int)rspeed);
		usleep(ST);
		pl_diff = l_diff;
		pr_diff = r_diff;
		perr = err;
		acc = 0;
	}

}
