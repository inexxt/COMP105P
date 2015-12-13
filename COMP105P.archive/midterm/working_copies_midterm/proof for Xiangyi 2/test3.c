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
float Ke = 0.8, Ke_d = 0.4;
float Kd = 0.1, Kd_d = 0.2;

double ldist = 0, rdist = 0, lpreviousdist = 0, rpreviousdist = 0; 
double d = 0; //The distance done between now and previous ST

double l_opt_dist = 0, r_opt_dist = 0;

int i = 0, sig = 0;
int var = 1;

float start_rangle = 50, start_langle = 20; 
float rangle = 0, langle = 0; 

double robot_angle = 0;
double curV = 0, curH = 0;

double wheelDtoCm(double d)
{
	double x = d/360*M_PI*R_WHEEL;
	return x;
}

void update_list()
{	
	ldist = leftencoder();
	rdist = rightencoder();
	
	push(head_M, wheelDtoCm(ldist-lpreviousdist), wheelDtoCm(rdist - rpreviousdist));
// 	printf("\tAAA %.2f %.2f", wheelDtoCm(ldist-lpreviousdist), wheelDtoCm(rdist - rpreviousdist));
	lpreviousdist = ldist;
	rpreviousdist = rdist;
}

void update_position()
{	
	ldist = leftencoder();
	rdist = rightencoder();
	
	double dl = wheelDtoCm(ldist-lpreviousdist);
	double dr = wheelDtoCm(rdist - rpreviousdist);
	
	double curAngle = computeAngle(dl, dr);
	robot_angle += curAngle;
	
	double cw = computeWay(dl, dr, curAngle);
	curV = curV + cw*cos(robot_angle);
	curH = curH + cw*sin(robot_angle);
	
	lpreviousdist = ldist;
	rpreviousdist = rdist;
	
	printf("CURRENT POSITION: V\t%.2f\tH\t%.2fA\t%.2f\n", curV, curH, robot_angle);
}

int checkifnot90() //wrong, I didn't take into account the fact that I can turn
{
// 	double eps = 0.05;
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

double way_angle(int index, int howfar1, int howfar2)
{
	node_t * B = list_get(index + howfar1);
	node_t * C = list_get(index + howfar2);
	
	if(distance(C->l, B->l, C->r, B->r) == 0) return 0;
	
	return asin((C->r - B->r)/distance(C->l, B->l, C->r, B->r));
}

int two_vectors_cross(double ax, double ay, double bx, double by, double cx, double cy)
{
	double v1x = ax - bx;
	double v1y = ay - by;
	double v2x = cx - bx;
	double v2y = cy - by;
	
// 	printf("vec1: x %.2f y %.2f\n", v1x, v1y);
// 	printf("vec2: x %.2f y %.2f\n", v2x, v2y);
// 	
// 	printf("ax %.2f ay %.2f\n", ax, ay);
// 	printf("bx %.2f by %.2f\n", bx, by);
// 	printf("cx %.2f cy %.2f\n", cx, cy);
	return -signprim(v1x * v2y - v2x * v1y);
}

double errorprim(int index, int howfar1, int howfar2)
{
	node_t * B = list_get(index + howfar1);
	node_t * C = list_get(index + howfar2);

	double signum = two_vectors_cross(curV, curH, B->l, B->r, C->l, C->r);
	return distance(curV, B->l, curH, B->r) * signum;
}

double error(int index, int howfar)
{
	node_t * B = list_get(index + howfar);
	return distance(curV, B->l, curH, B->r);
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
	
// 	printf("\t\tL %.2f\tR %.2f\ti %d\n", langle, rangle, i);
	
	set_ir_angle(RIGHT, (int)(-135+rangle));
	set_ir_angle(LEFT, (int)(-45+langle));
	
	degrees_to_radians();
}

int main()
{
	
// 	printf("test 1 : %d\n", two_vectors_cross(1,2,3,1,4,5));
// 	printf("test 2 : %d\n", two_vectors_cross(3,1,1,2,4,5));
// 	printf("test 3 : %d\n", two_vectors_cross(1,2,4,5,3,1));
// 	printf("test 4 : %d\n", two_vectors_cross(4,5,1,2,3,1));
	
	
	connect_to_robot();
	initialize_robot();
	
	set_origin();
	
	lpreviousdist = leftencoder();
	rpreviousdist = rightencoder();
	
	head_M = initialize_list(leftFsensor(), rightFsensor());
	
	set_ir_angle(RIGHT, -135+start_rangle);
	set_ir_angle(LEFT, -45+start_langle);
	
	sleep(1);
	
	degrees_to_radians();
	
	while(var)
	{
		int iaa = rightFsensor();
		
		if (iaa < 40)
		{
			int e = get_us_dist();
			printf("e is %d,i is %d",e,iaa);
			if ((e-5)>=iaa-2 && (e-5)<=iaa+3)
			{
				iaa=rightFsensor();
				var = 0;
				break;
			}
		}
		
		l_opt_dist = OD/cos(langle);
		r_opt_dist = (OD+R_WIDTH)/cos(rangle);
		
		lspeed = BS;
		rspeed = BS;
		
		l_diff = l_opt_dist - leftFsensor(); //if positive turn right 
		r_diff = r_opt_dist - rightFsensor(); //if really positive turn right (fast escape!)
		
		y = (leftFsensor()*cos(langle) + R_WIDTH)/cos(langle);
		p = y*cos(langle)/cos(rangle);		
		err = rightFsensor() - p; //if positive turn left
		
		if(rightFsensor() < 40) acc -= Ke*(err - Ke_d*fabs(err - perr));
		if(leftFsensor() < 40)
		{
			acc += Kd*(l_diff*fabs(l_diff) - Kd_d*fabs(l_diff - pl_diff));
			acc += Ks*exp(r_diff- 30 - Ks_d*fabs(l_diff - pl_diff)); //differentiating l_diff, because it's more reliable
		}
		else
		{
			rspeed = 40;
			lspeed = 10;
		}
		
		var = checkifnot90();
		
		lspeed += acc;
		rspeed -= acc;
		
// 		log_trail();
		fuelcheck();
		update_angles();
		
		set_motors((int)lspeed, (int)rspeed);
		
		update_list();
		
		counter++;
		printf("%d\n", counter);
		
// 		if(counter == MAX_CTR) var = 0;
		
		usleep(ST);
		pl_diff = l_diff;
		pr_diff = r_diff;
		perr = err;
		acc = 0;
	}
	
	set_motors(0,0);
	print_list(head_M);
	
// 	printf("Angle %.2f\n", computeAngleList());
	computeWayList();
// 	printf("wV %.2f wH %.2f", wV, wH);
	
	print_list(head_XY);
	
	robot_angle = computeAngleList();
	
	turn(180);
	printf("\tRA1 %.2f", robot_angle);
	robot_angle = robot_angle - M_PI;
	
	printf("\tRA2 %.2f", robot_angle);
	scanf("%d", &var);
// 	robot_angle = 0;
	var = 1;
	curV = wV;
	curH = wH;
	
	lpreviousdist = leftencoder();
	rpreviousdist = rightencoder();
	
	int index = 0;
	double p1, p2, p3;
	double Kangle = 60, Kp1 = 2, err_a = 0, eps = 3;
	while(var)
	{
		lspeed = BS_RETURN; //Check if it will be necessary
		rspeed = BS_RETURN;
		
		while(robot_angle > 2*M_PI) robot_angle -= 2*M_PI;
		while(robot_angle < -2*M_PI) robot_angle += 2*M_PI;
		
		index = find_closest(curV, curH);
		
		printf("Index: %d\n", index);
		
		p1 = errorprim(index, CLOSE_RELATE/2, FAR_RELATE/2);
		p2 = error(index, CLOSE_RELATE); 
		p3 = error(index, FAR_RELATE);
		
		err_a = -robot_angle - way_angle(index, CLOSE_RELATE/2, FAR_RELATE/2) - M_PI;
		
		printf("p1 %.2f\n", p1);
		printf("err_a %.2f\n", err_a);
		
		if (fabs(p1) > 5) acc -= Kp1*p1;
// 		acc += Kp2*p2*signprim(p1);
// 		acc += Kp3*p3;
		acc += Kangle*err_a;
		
// 		if(fabs(acc) > 50) acc = 10;
		printf("ACC : %.2f\n", acc);
		
		lspeed += acc;
		rspeed -= acc;
		
// 		log_trail();
// 		fuelcheck();
		
		set_motors((int)lspeed, (int)rspeed);
		
// 		if(abs(index - MAX_CTR) < FAR_RELATE) var = 0;
		if(abs(index - MAX_CTR) < eps) var = 0;
		
		update_position();
		
		usleep(ST);
		pl_diff = l_diff;
		pr_diff = r_diff;
		perr = err;
		acc = 0;
	}
}