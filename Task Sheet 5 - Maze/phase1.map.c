#include "phase1.map.h"

extern double bearing;

//why do we need so many global vars? J.
int frontLeftIR = 0;
int frontRightIR = 0;
int sideLeftIR = 0;
int sideRightIR = 0;
int usValue = 0; //what's that? J.



extern Queue* nonVisited;
extern Queue* currentPath;

Queue* pathTo(XY dest)
{
	Queue* path = NULL;
	//BFS on maze
	return path;
}

XY popFront(Queue** q) //pointer to pointer, because we want to modify the real queue
{
	XY sxy = (*q)->sxy;
	*q = (*q)->next;
	return sxy;
}

void pushBack(Queue* q, XY sxy) //not sure about this one (i don't know if malloc will modify q or just local copy)
{
	Queue* current = q;
    while (current != NULL) 
	{
        current = current->next;
    }
    current = malloc(sizeof(Queue));
    current->sxy = sxy;
    current->next = NULL;
}

int isEmpty(Queue* q)
{
	if(q == NULL) return 1;
	else return 0;
}

XY popNonVisited(Queue* q)
{
	if(isEmpty(q))
	{
		XY cs = getCurrentSector();
		if(cs.x == 0 && cs.y == 0) return (XY){.x = 0, .y = -1}; //if we have nowhere else to go and we are already in (0,0), that's the end
		else return (XY){.x = 0, .y = 0}; //we have visited all sectors, so we have to return to (0,0)
	}
	return popFront(&q);
}

XY nextSector()
{
	if(isEmpty(currentPath))
	{
		XY goal_dest = popNonVisited(nonVisited);
		if(goal_dest.y == -1) return goal_dest; //it means that we don't have any unvisited sectors and we had returned to (0,0)
		currentPath = pathTo(goal_dest);
	}
	return popFront(&currentPath);
}



void updateSector()
{
	XY currSect = getCurrentSector();
	int x = currSect.x;
	int y = currSect.y;
	
	if(maze[x][y].visited == 0)
	{
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
		
		//////////////////////////////////////////////////
		
		
		
		maze[x][y] = (Sector){.northWall = northWall, .southWall = southWall, .eastWall = eastWall, .westWall = westWall, .visited = 1};
		
		if(northWall == 0 && maze[x][y + 1].visited == 0) pushBack(nonVisited, (XY){.x = x, .y = y + 1});
		if(southWall == 0 && maze[x][y - 1].visited == 0) pushBack(nonVisited, (XY){.x = x, .y = y - 1});
		if(eastWall == 0 && maze[x + 1][y].visited == 0) pushBack(nonVisited, (XY){.x = x + 1, .y = y});
		if(westWall == 0 && maze[x - 1][y].visited == 0) pushBack(nonVisited, (XY){.x = x - 1, .y = y});
	}
}