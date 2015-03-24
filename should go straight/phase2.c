#include "defines.h"
#include "phase1.h"
#include "phase1.map.h"
#include "phase2.h"

// void followPath() //TODO add Queue* support void followPath(Queue* path)
// {
//   double xDifference;
//   double yDifference;
//   double requiredAngleChange;
//   double remainingDistance;
//   int start = 1;
//   size -= 40;
//   while (size > 0) // while there are still stored points left
//   {
//     log_trail();
//     updateRobotPosition(&bearing, &xPos, &yPos);
//   	xDifference = *(xStorage+size) - xPos;
//     yDifference = *(yStorage+size) - yPos;
//     requiredAngleChange = atan2(xDifference,yDifference) - bearing;
//     remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);
// 
//     /*So the robot would operate in a -pi to +pi range: */
//   	if(requiredAngleChange > (M_PI))
//   	  requiredAngleChange -= (2*M_PI);
//   	if(requiredAngleChange < (-M_PI))
//   	  requiredAngleChange += (2*M_PI);
// 
//     if((fabs(xDifference) < 5.0) && (!start)) // start ensures the robot will rotate at the beginning regardless of relative position
//     {
//     	if(yDifference > 0)
//     		requiredAngleChange = 0;
//     	else
//     		requiredAngleChange = M_PI;
//     	start = 0;
//     }
// 
//   	/**********DEBUG**********/
//     printf("\nCurrently at... %f     %f\n",xPos,yPos);
//     printf("Bearing: %f\n", bearing);
//     printf("going to... %f     %f\n", (*(xStorage+size)),(*(yStorage+size)));
//     set_point(*(xStorage+size)/10.00,*(yStorage+size)/10.00);  // draw the point the robot is going to
//     printf("Remaining distance: %f\n", remainingDistance);
//     printf("xDif: %f\tyDif: %f\n", xDifference, yDifference);
//     printf("\nRequired angle change: %f\n", requiredAngleChange);
//     /**********DEBUG**********/
//      
//     if(remainingDistance > 100) // breaks the loop in case the robot goes wild
//       return;
// 
//     if(remainingDistance < 7)
//     {
//     	double distanceFromNextPoint = 0.0;
//     	while(distanceFromNextPoint < MINIMUM_DISTANCE_BETWEEN_POINTS/10)
//     	{
//     	  xDifference = *(xStorage+size) - xPos;
//  		  yDifference = *(yStorage+size) - yPos;
//  		  distanceFromNextPoint = sqrt(xDifference*xDifference + yDifference*yDifference);
//  		  size--;
//  		  if(size == 1) //ensures robot will go to the starting position
//     	  	break;
//     	}
//     }
// 
//     if(fabs(requiredAngleChange) > 1.8) // for sharp turns, in practice only for turning back at the beginning
//     {
//     	while(fabs(requiredAngleChange) > 0.30)
//     	{
//     	  log_trail();
//           updateRobotPosition(&bearing, &xPos, &yPos);
//           requiredAngleChange = atan2(xDifference,yDifference) - bearing;
//           set_motors(requiredAngleChange*14.0,-requiredAngleChange*14.0);
//     	}
//     }
//     else
//     {
//       log_trail();
//       updateRobotPosition(&bearing, &xPos, &yPos);
//       set_motors((MEDIUM_SPEED + MEDIUM_SPEED * requiredAngleChange*1.2),(MEDIUM_SPEED - MEDIUM_SPEED * requiredAngleChange*1.2));
//     }
//   }
// }

typedef struct
{
	XY act;
	int prev;
} Elem;

int visited[4][4]; //1 - visited, 2 - added to q

int available(XY s, XY sp, int dir)
{	
	int x = s.x;
	int y = s.y;
	int xp = sp.x;
	int yp = sp.y;
// 	printf("AVAIL X %d Y %d type %d\n", x, y, type);
	if(x > MAZE_WIDTH - 1 || x < 0 || y > (MAZE_HEIGHT - 1) || y < 0) return 0;
	if(dir == 1 && maze[xp][yp].northWall != 0) return 0;
	if(dir == 2 && maze[xp][yp].southWall != 0) return 0;
	if(dir == 3 && maze[xp][yp].westWall != 0) return 0;
	if(dir == 4 && maze[xp][yp].eastWall != 0) return 0;
	if(visited[x][y] != 0) return 0; //checking if visited only if i want to explore it - if that's my return path, do not do that
	printf("Adding (%d %d) from (%d %d)\n", x,y,xp,yp);
	return 1;
}

Queue* calculateOptimalPath()
{
	Queue* oP = NULL;
	XY cur = {.x = 0, .y = 0};
	
	int i, j;
	for(i = 0; i<4; i++)
		for(j = 0; j<4; j++)
			visited[i][j] = 0;
	
	Elem q[16];
	int counter = 1;
	int currctr = 0;
	q[0].act = cur;
	q[0].prev = -1;
	
	while(!(cur.x == 3 && cur.y == 3))
	{
		int x = cur.x;
		int y = cur.y;
		visited[x][y] = 1;
		printf("Managing %d %d currctr %d counter %d\n", x, y, currctr, counter);
		
		if(available((XY){x+1, y}, (XY){x,y}, 4))
		{
			q[counter++] = (Elem){(XY){x+1, y}, currctr};
			visited[x+1][y] = 2;
		}
		if(available((XY){x-1, y}, (XY){x,y}, 3))
		{
			q[counter++] = (Elem){(XY){x-1, y}, currctr};
			visited[x-1][y] = 2;
		}
		if(available((XY){x, y+1}, (XY){x,y}, 1))
		{
			q[counter++] = (Elem){(XY){x, y+1}, currctr};
			visited[x][y+1] = 2;
		}
		if(available((XY){x, y-1}, (XY){x,y}, 2))
		{
			q[counter++] = (Elem){(XY){x, y-1}, currctr};
			visited[x][y-1] = 2;
		}
		currctr++;
		cur = q[currctr].act;
	}
	
	for(i = 0; i<16; i++)
		printf("q[%d] = (Elem){(XY){%d, %d}, %d};\n", i, q[i].act.x, q[i].act.y, q[i].prev);
	
	counter--;
	while(!(cur.x == 0 && cur.y == 0))
	{
		printf("counter %d\n", counter);
		pushFront(&oP, cur);
		counter = q[counter].prev;
		cur = q[counter].act;
	}
	pushFront(&oP, (XY){0,0});
	
	printQueue(oP);
	return oP;
}

