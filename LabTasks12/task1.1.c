#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"

int main(){
	connect_to_robot();
	initialize_robot();
	set_motors(10,10);
	sleep(2);
}
