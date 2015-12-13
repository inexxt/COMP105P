

void followPath() //TODO add Queue* support void followPath(Queue* path)
{
  double xDifference;
  double yDifference;
  double requiredAngleChange;
  double remainingDistance;
  int start = 1;
  size -= 40;
  while (size > 0) // while there are still stored points left
  {
    log_trail();
    updateRobotPosition(&bearing, &xPos, &yPos);
  	xDifference = *(xStorage+size) - xPos;
    yDifference = *(yStorage+size) - yPos;
    requiredAngleChange = atan2(xDifference,yDifference) - bearing;
    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);

    /*So the robot would operate in a -pi to +pi range: */
  	if(requiredAngleChange > (M_PI))
  	  requiredAngleChange -= (2*M_PI);
  	if(requiredAngleChange < (-M_PI))
  	  requiredAngleChange += (2*M_PI);

    if((fabs(xDifference) < 5.0) && (!start)) // start ensures the robot will rotate at the beginning regardless of relative position
    {
    	if(yDifference > 0)
    		requiredAngleChange = 0;
    	else
    		requiredAngleChange = M_PI;
    	start = 0;
    }

  	/**********DEBUG**********/
    printf("\nCurrently at... %f     %f\n",xPos,yPos);
    printf("Bearing: %f\n", bearing);
    printf("going to... %f     %f\n", (*(xStorage+size)),(*(yStorage+size)));
    set_point(*(xStorage+size)/10.00,*(yStorage+size)/10.00);  // draw the point the robot is going to
    printf("Remaining distance: %f\n", remainingDistance);
    printf("xDif: %f\tyDif: %f\n", xDifference, yDifference);
    printf("\nRequired angle change: %f\n", requiredAngleChange);
    /**********DEBUG**********/
     
    if(remainingDistance > 100) // breaks the loop in case the robot goes wild
      return;

    if(remainingDistance < 7)
    {
    	double distanceFromNextPoint = 0.0;
    	while(distanceFromNextPoint < MINIMUM_DISTANCE_BETWEEN_POINTS/10)
    	{
    	  xDifference = *(xStorage+size) - xPos;
 		  yDifference = *(yStorage+size) - yPos;
 		  distanceFromNextPoint = sqrt(xDifference*xDifference + yDifference*yDifference);
 		  size--;
 		  if(size == 1) //ensures robot will go to the starting position
    	  	break;
    	}
    }

    if(fabs(requiredAngleChange) > 1.8) // for sharp turns, in practice only for turning back at the beginning
    {
    	while(fabs(requiredAngleChange) > 0.30)
    	{
    	  log_trail();
          updateRobotPosition(&bearing, &xPos, &yPos);
          requiredAngleChange = atan2(xDifference,yDifference) - bearing;
          set_motors(requiredAngleChange*14.0,-requiredAngleChange*14.0);
    	}
    }
    else
    {
      log_trail();
      updateRobotPosition(&bearing, &xPos, &yPos);
      set_motors((MEDIUM_SPEED + MEDIUM_SPEED * requiredAngleChange*1.2),(MEDIUM_SPEED - MEDIUM_SPEED * requiredAngleChange*1.2));
    }
  }
}

Queue* calculateOptimalPath()
{
	Queue* oP = NULL;
	XY current = {.x = -1, .y = 0};
	
	return oP;
}
