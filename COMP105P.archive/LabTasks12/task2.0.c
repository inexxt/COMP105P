#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include <math.h>
#define D360 1697
#define M1 754

int left, right;

int leftencoder(){
        get_motor_encoders(&left, &right);
        return left;
}

int rightencoder(){
        get_motor_encoders(&left, &right);
        return right;
}

void turn(int k){
        int Linv = abs(leftencoder());
	int Rinv = abs(rightencoder()); 
        int currentvalue = 0;
        //printf("I %d C %d\n", initialvalue, currentvalue);
        while (currentvalue < D360*k/360)
        {
                set_motors(5, -5);
                //nanosleep((struct timespec[]){{0, 10000000}}, NULL);
                usleep(10000);
                //log_trail();
                currentvalue=abs(leftencoder() - Linv) + abs(rightencoder() - Rinv);
		printf("L %d R %d C %d\n", leftencoder(), rightencoder(), currentvalue);
        }
        set_motors(0, 0);
        sleep(1);
}

void straight(int a){
        int initialvalue = leftencoder();
        int currentvalue = 0;
        printf("I %d C %d\n", initialvalue, currentvalue);
        while (currentvalue < initialvalue + M1*a)
        {
                set_motors(30,30);
                //nanosleep((struct timespec[]){{0, 10000000}}, NULL);
                usleep(100000);
                //log_trail();
                currentvalue=leftencoder();
                printf("R C %d I %d\n", currentvalue, initialvalue);
        }
        set_motors(0,0);
        sleep(1);
}

int main(){
	int i=0;
	connect_to_robot();
	initialize_robot();
	straight(1);
	turn(90);
	straight(1);
	turn(135);
	straight(1.414);
	turn(135);
}
