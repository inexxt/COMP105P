#include "phase1.map.h"
#define RETURN 1
#define EXPLORING 2

extern double bearing;

//why do we need so many global vars? J.
int frontLeftIR = 0;
int frontRightIR = 0;
int sideLeftIR = 0;
int sideRightIR = 0;
int usValue = 0; //what's that? J.

extern Queue* nonVisited;
extern Queue* currentPath;

XY popFront(Queue** q) //pointer to pointer, because we want to modify the real queue
{
	XY sxy = (*q)->sxy;
	*q = (*q)->next;
	return sxy;
}

void pushBack(Queue* q, XY sxy, int type) //not sure about this one (i don't know if malloc will modify q or just local copy)
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
		if(cs.x == 0 && cs.y == -1) return (XY){.x = -1, .y = -1}; //if we have nowhere else to go and we are already in (0,-1), that's the end
		else 
		{
			printf("\tERROR\tERROR\tERROR\tERROR\n");
			printf("%d %d \n", cs.x, cs.y);
			return (XY){.x = -100, .y = -100}; //something went wrong, that situations shoudn't occur
	}
	return popFront(&q);
}

XY nextSector()
{
	if(isEmpty(currentPath)) return (XY){.x = 0, .y = -1};
	return popFront(&currentPath);
}

int availSector(int x, int y, int type)
{
	
// 	static int maze_indicators_init = 0;
// 	static int maze_indicators[MAZE_WIDTH][MAZE_HEIGHT];
// 	
// 	if(maze_indicators_init == 0)
// 	{
// 		maze_indicators_init == 1;
// 		int i = 0, j = 0;
// 		for(i = 0; i<MAZE_WIDTH; i++)
// 		{
// 			for(j = 0; j<MAZE_HEIGHT; j++)
// 			{
// 				maze_indicators[i][j] = 0;
// 			}
// 		}
// 	}
	
	if(x > MAZE_WIDTH - 1 || x < 0 || y > MAZE_HEIGHT - 1 || y < 0) return 0;
	if(maze[x][y].visited == 1 && type == EXPLORING) return 0; //checking if visited only if i want to explore it - if that's my return path, do not do that
	
	return 1;
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
		
		//add previous location - the place we just left, with type = RETURN, instead of some north/south etc
		//DANGER add it on the beginning, before anything else
		
		if(northWall == 0 && availSector(x, y+1, EXPLORING) == 1) pushBack(nonVisited, (XY){.x = x, .y = y+1});
		if(southWall == 0 && availSector(x, y-1, EXPLORING) == 1) pushBack(nonVisited, (XY){.x = x, .y = y-1});
		if(eastWall == 0 && availSector(x+1, y, EXPLORING) == 1) pushBack(nonVisited, (XY){.x = x+1, .y = y});
		if(westWall == 0 && availSector(x-1, y, EXPLORING) == 1) pushBack(nonVisited, (XY){.x = x-1, .y = y});
	}
}