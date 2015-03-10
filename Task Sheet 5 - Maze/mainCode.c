#include "defines.h"
#include "phase1.h"
#include "phase1.map.h"
#include "phase1.move.h"
//#include "phase2.h"




int main() 
{
  connect_to_robot();
  initialize_robot();
  

    set_motors(-10,-10);  // SUPER TEMP TODO (for some reason x,y update there, lol);
    sleep(2); // tmep
reset_motor_encoders(); // temp
  set_origin();
  xPos = 0.0000;
  yPos = 0.0000;
  bearing = 0.0000;

  set_ir_angle(LEFT, 0);
  set_ir_angle(RIGHT, -75);   
  struct XY test = {.x = 0, .y = 2} ;

  goToSector(test);

  set_motors(0,0);


  return 0;
}