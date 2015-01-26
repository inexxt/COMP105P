#include <stdio.h>
#include ‘‘picomms.h’’

int main() 
{
	int speed = 10;
	connect_to_robot();
	initialize_robot();
	set_motor(LEFT, speed);
	log_trail();
}