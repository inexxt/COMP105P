#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"

int main(){
	int speed = 102;
    int i=0;
	connect_to_robot();
	initialize_robot();
    set_motor(LEFT,speed);
	sleep(2);
}
