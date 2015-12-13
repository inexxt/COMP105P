#include "phase1.map.h"
#include "recordPosition.h"
#include "basicMovement.h"

void goToXY(XY destination, int phase);
void endPhase1();
void rotateTowardsTarget(double targetX, double targetY, int phase);
void moveToTarget(double targetX, double targetY, int phase);