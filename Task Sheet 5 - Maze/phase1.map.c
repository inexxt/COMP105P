#include "phase1.map.h"

double bearing;

//why do we need so many global vars?
int frontLeftIR = 0;
int frontRightIR = 0;
int sideLeftIR = 0;
int sideRightIR = 0;


int usValue = 0; //what's that? J.

/*
void initialiseMaze()
{
	int i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			maze[i][j] = malloc(sizeof(Sector));
		}
	}
}
*/

// struct XY nextSector()
// {
// 	if(isEmpty(currentPath))
// 	{
// 		struct XY goal_dest = popNonVisited(nonVisitedQueue);
// 		if(goal_dest.y == -1) return goal_dest; //it means that we don't have any unvisited sectors and we had returned to (0,0);
// 		struct Queue temp_path = pathTo(goal_dest);
// 		joinQueues(current_path, temp_path);
// 	}
// 	return popFirst(currentPath);
// }
// 
// struct XY popNonVisited(Queue q)
// {
// 	
// 	if(isEmpty(q))
// 	{
// 		struct XY cs = getCurrentSector();
// 		if(cs.x == 0 && cs.y == 0) return (XY){} //TODO}
// 	}
// }

void updateSector()
{
	XY currentSector = getCurrentSector();
	set_ir_angle(LEFT, -45);
	set_ir_angle(RIGHT, 45);  
	sleep(1);
	int frontLeftReading, frontRightReading, sideLeftReading, sideRightReading, ultraSound;
	get_front_ir_dists(&frontLeftReading, &frontRightReading);
	get_side_ir_dists(&sideLeftReading, &sideRightReading);
	ultraSound = get_us_dist();

	int northWall = 0;
	int southWall = 0;
	int westWall = 0;
	int eastWall = 0;

  	//bearing divider
	//TODO INCLUDE BEARING!!!
 	if((frontLeftReading < SECTOR_WIDTH/2) && (sideLeftReading < SECTOR_WIDTH/2))
 	{
		westWall = 1;
 	}
 	if((frontRightReading < SECTOR_WIDTH/2) && (sideRightReading < SECTOR_WIDTH/2))
 	{
      eastWall = 1;
 	}

	set_ir_angle(LEFT, 45);
  	set_ir_angle(RIGHT, -45);  
  	sleep(1);
    get_front_ir_dists(&frontLeftReading, &frontRightReading);

	
	maze[currentSector.x][currentSector.y].northWall = northWall;
	maze[currentSector.x][currentSector.y].southWall = southWall;
	maze[currentSector.x][currentSector.y].westWall = westWall;
	maze[currentSector.x][currentSector.y].eastWall = eastWall;
	maze[currentSector.x][currentSector.y].visited = 1;
}