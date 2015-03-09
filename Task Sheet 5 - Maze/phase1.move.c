double xPos, yPos, bearing;



void goToXY(struct XY destination)
{
  double xDifference;
  double yDifference;
  double requiredAngleChange;
  double remainingDistance;
  while (size > 0) //TODO
  {
    updateRobotPosition(&bearing, &xPos, &yPos); //TODO
  	xDifference = destination.x - xPos;
    yDifference = destination.y - yPos;
    requiredAngleChange = atan2(xDifference,yDifference) - bearing;
    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);

    if(remainingDistance < 20)
    {
    	// TODO
    }

    if(fabs(requiredAngleChange) > 1.2) // for sharp turns, in practice only for turning back at the beginning
    {
    	while(fabs(requiredAngleChange) > 0.10)
    	{
          updateRobotPosition(&bearing, &xPos, &yPos); //TODO
          requiredAngleChange = atan2(xDifference,yDifference) - bearing;
          set_motors(requiredAngleChange*14.0,-requiredAngleChange*14.0);
    	}
    }
    else
    {
      updateRobotPosition(&bearing, &xPos, &yPos); // TODO
      set_motors((MEDIUM_SPEED + MEDIUM_SPEED * requiredAngleChange*1.2),(MEDIUM_SPEED - MEDIUM_SPEED * requiredAngleChange*1.2));

      // TODO: add sensor correction
    }
  }
}

void goToSector(struct XY destination)
{
	<complicatedStuff>
	goToXY(...);
	goToXY(...);
}