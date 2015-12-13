#include <stdio.h>
#include <stdlib.h>
#include "picomms.h"

int sensoroutput1()
{
	int value1;
	two_sensor_read(&sensornames, &value1, &value2);
	return value1;
}

int sensoroutput2(){
	int value2;
	two_sensor_read(&sensornames, &value1, &value2);
	return value2;
}
