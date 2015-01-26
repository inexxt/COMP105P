#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"
#include "static_functions.h"

int main(){
	connect_to_robot();
	initialize_robot();
	straight(1);
	turn(90);
	straight(1);
	turn(135);
	straight(1.414);
	turn(135);
}
