#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include "static_functions.h"
#include "lists.h"
#include <math.h>
#include "defines.h"

int counter = 0;

float fuel = 2.2*BS;

float lspeed = BS, rspeed = BS;
float acc = 0, err=0, l_diff = 0, r_diff = 0, pl_diff = 0, pr_diff = 0, perr = 0,  p, y;
float Ks = 0.8, Ks_d = 0.5;
float Ke = 0.6, Ke_d = 0.4;
float Kd = 0.1, Kd_d = 0.2;


double ldist = 0, rdist = 0, lpreviousdist = 0, rpreviousdist = 0; 
float d = 0; //The distance done between now and previous ST

float l_opt_dist = 0, r_opt_dist = 0;

int i = 0, sig = 0;
int var = 1;


float start_rangle = 50, start_langle = 20; 
float rangle = 0, langle = 0; 

node_t * head = NULL;
	
void initialize_list(int l, int r)
{
	head = malloc(sizeof(node_t));
	head->l = l;
	head->r = r;
	head->next = NULL;
}

void update_list()
{	
	ldist = leftencoder();
	rdist = rightencoder();
	
	push(head, ldist-lpreviousdist, rdist - rpreviousdist);
	
	lpreviousdist = ldist;
	rpreviousdist = rdist;
}

int checkifnot90() //wrong, I didn't take into account the fact that I can turn
{
// 	double eps = 0.05;
// 	
// 	
// 	double dl = lastL(head);
// 	double lalpha = langle - pi/2;
// 	double lc = thirdSideOfTriangle(l_diff, d, lalpha);
// 	double lbeta = secondAngle(lc, lalpha, l_diff);
// 	double lgamma = pi - lalpha - lbeta;
// 	double ldc = thirdSideOfTriangle(lc, pl_diff, lgamma);
// 	double ldelta = secondAngle(ldc, lgamma, lc);
// 	
// 	double dr = lastR(head);
// 	double ralpha = rangle + pi/2;
// 	double rc = cthirdSideOfTriangle(d, r_diff, ralpha);
// 	double rbeta = secondAngle(rc, ralpha, r_diff);
// 	double rgamma = pi - ralpha - rbeta - pi/2;
// 	double rdc = thirdSideOfTriangle(rc, pr_diff, rgamma);
// 	double rdelta = pi - secondAngle(rdc, rgamma, rc);
// 	
// 	double ang = ldelta + rdelta + (pi - langle) + rangle;
// 	if (fabs(ang - pi/2) < eps) return 0;
// 	return 1;
	return 1;
// 	double sl = leftSsensor();
// 	double fl = leftFsensor();
// 	double fr = rightFsensor();
// 	
// 	double sinL = sl / fl;
// 	double sinR = (fr+R_WIDTH)/fr;
}

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
	rangle = rangle*2*M_PI/360.0;
	langle = langle*2*M_PI/360.0;
}

void update_angles()
{
	i += sig;
	
	if(i%5 == 0)
	{
		sig = -sig;
	}
	langle = start_langle - i;
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
	set_origin();
	
	initialize_list(leftFsensor(),rightFsensor());
	
	set_ir_angle(RIGHT, -135+start_rangle);
	set_ir_angle(LEFT, -45+start_langle);
	
	sleep(1);
	
	degrees_to_radians();
	
	while(var)
	{
		l_opt_dist = OD/cos(langle);
		r_opt_dist = (OD+R_WIDTH)/cos(rangle);
		
		lspeed = BS;
		rspeed = BS;
		
		l_diff = l_opt_dist - leftFsensor(); //if positive turn right 
		r_diff = r_opt_dist - rightFsensor(); //if really positive turn right (fast escape!)
		
		y = (leftFsensor()*cos(langle) + R_WIDTH)/cos(langle);
		p = y*cos(langle)/cos(rangle);		
		err = rightFsensor() - p; //if positive turn left
		
// 		printf("RCOS %.2f\tLCOS %.2f R %.2f L%.2f\n", cos(rangle), cos(langle), rangle, langle);
// 		printf("Y %.2f\tP %.2f ERR %.2f\n", y, p, err);
		
		if(rightFsensor() < 40) acc -= Ke*(err - Ke_d*fabs(err - perr));
		if(leftFsensor() < 40)
		{
			acc += Kd*(l_diff*fabs(l_diff) - Kd_d*fabs(l_diff - pl_diff));
			acc += Ks*exp(r_diff- 30 - Ks_d*fabs(l_diff - pl_diff)); //differentiating l_diff, because it's more reliable
		}
		else
		{
			rspeed = 40;
			lspeed = 15;
		}
		
		var = checkifnot90();
		
		lspeed += acc;
		rspeed -= acc;
		
		log_trail();
		fuelcheck();
		update_angles();
		
		set_motors((int)lspeed, (int)rspeed);
		
		update_list();
		
		counter++;
		printf("\n%d\n", counter);
		
		if(counter == 100) var = 0;
		
		usleep(ST);
		pl_diff = l_diff;
		pr_diff = r_diff;
		perr = err;
		acc = 0;
	}
	
	print_list(head);
	printf("Angle %.2f wV %.2f wH %.2f\n", computeAngleList(head)/360*2*M_PI, computeWayList(head, VERTICAL), computeWayList(head, HORIZONTAL));
	
}
