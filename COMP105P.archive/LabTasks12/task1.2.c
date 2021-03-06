#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"

void turn(int n){
	int i=0;
	for(i=0;i<60;i++){
		int s = (100*n)/360;
		printf("%d",s);
		set_motor(LEFT, s);
		printf("SOMETHING");
		nanosleep((struct timespec[]){{0, 10000000}}, NULL);
		log_trail();
	}
	set_motor(LEFT, 0);
}

void straight(int a){
	int i=0;
	for(i=0;i<200;i++){
	 set_motors(a,a);
	 nanosleep((struct timespec[]){{0, 10000000}}, NULL);
	 log_trail();
	}
	set_motors(0,0);
}

int main(){
	int i=0;
	connect_to_robot();
	initialize_robot();
	for(i=0;i<4;i++){
       turn(90);
	   straight(10);
	}
}

