#include "phase1.map.h"
#define RETURN 1
#define EXPLORING 0
#define SECTOR_WIDTH_SECTOR 60
extern double bearing;

extern Queue* currentPath;

XY popFront(Queue** q) //pointer to pointer, because we want to modify the real queue
{
	XY sxy = (*q)->sxy;
	*q = (*q)->next;
	return sxy;
}

void pushBack(Queue* q, XY sxy) //not sure about this one (i don't know if malloc will modify q or just local copy)
{
	printf("ADDED sxy x %d y %d\n", sxy.x, sxy.y);
	maze[sxy.x][sxy.y].visited = 1;
	
	Queue* current = q;
    while (current->next != NULL) 
	{
        current = current->next;
    }
    current->next = malloc(sizeof(Queue));
    current->next->sxy = sxy;
    current->next->next = NULL;
}

int isEmpty(Queue* q)
{
	if(q == NULL) return 1;
	else return 0;
}

XY popcurrentPath(Queue* q)
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
	}
	return popFront(&q);
}

XY nextSector()
{
	if(isEmpty(currentPath))
	{
		printf("QUEUE IST EMPTY!!\n");
		return (XY){.x = 0, .y = -1};
	}
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

double convertToDegrees(double radians)
{
	return radians*180/M_PI;
}

// void rotate(int* n, int* w, int* s, int* e, double angle)
// {
// 	
// }

void updateSector()
{
	XY currSect = getCurrentSector();
	
	int x = currSect.x;
	int y = currSect.y;
	printf("CURRENT SECTOR is %d %d\n", x, y);
	
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
		
		int northType = 0;
		int southType = 0;
		int westType = 0;
		int eastType = 0;
		
// 		a[i] = a[(i + bearing*2/M_PI)%4]
		
		if(convertToDegrees(bearing) < 15 || convertToDegrees(bearing) > 345) //first case - it's heading north
		{
				
			if((frontLeftReading < SECTOR_WIDTH_SECTOR/2) && (sideLeftReading < SECTOR_WIDTH_SECTOR/2))
			{
				westWall = 1;
			}
			if((frontRightReading < SECTOR_WIDTH_SECTOR/2) && (sideRightReading < SECTOR_WIDTH_SECTOR/2))
			{
				eastWall = 1;
			}
			if(ultraSound < SECTOR_WIDTH_SECTOR/2)
			{
				northWall = 1;
			}
			southType = RETURN;
		}
		
		if(convertToDegrees(bearing) < 105 && convertToDegrees(bearing) > 75) //first case - it's heading north
		{
				
			if((frontLeftReading < SECTOR_WIDTH_SECTOR/2) && (sideLeftReading < SECTOR_WIDTH_SECTOR/2))
			{
				northWall = 1;
			}
			if((frontRightReading < SECTOR_WIDTH_SECTOR/2) && (sideRightReading < SECTOR_WIDTH_SECTOR/2))
			{
				southWall = 1;
			}
			if(ultraSound < SECTOR_WIDTH_SECTOR/2)
			{
				eastWall = 1;
			}
			westType = RETURN;
		}
		
		if(convertToDegrees(bearing) < 195 && convertToDegrees(bearing) > 165) //first case - it's heading north
		{
				
			if((frontLeftReading < SECTOR_WIDTH_SECTOR/2) && (sideLeftReading < SECTOR_WIDTH_SECTOR/2))
			{
				eastWall = 1;
			}
			if((frontRightReading < SECTOR_WIDTH_SECTOR/2) && (sideRightReading < SECTOR_WIDTH_SECTOR/2))
			{
				westWall = 1;
			}
			if(ultraSound < SECTOR_WIDTH_SECTOR/2)
			{
				southWall = 1;
			}
			northType = RETURN;
		}
		
		if(convertToDegrees(bearing) < 285 && convertToDegrees(bearing) > 255) //first case - it's heading north
		{
				
			if((frontLeftReading < SECTOR_WIDTH_SECTOR/2) && (sideLeftReading < SECTOR_WIDTH_SECTOR/2))
			{
				southWall = 1;
			}
			if((frontRightReading < SECTOR_WIDTH_SECTOR/2) && (sideRightReading < SECTOR_WIDTH_SECTOR/2))
			{
				northWall = 1;
			}
			if(ultraSound < SECTOR_WIDTH_SECTOR/2)
			{
				westWall = 1;
			}
			eastType = RETURN;
		}
		
		printf("DEBUG\tN %d S %d E %d W %d\n", northType, southType, eastType, westType);
		printf("WALLS\tN %d S %d E %d W %d\n", northWall, southWall, eastWall, westWall);
		printf("ULTRA %d\n", ultraSound);
// 		/*rotate(&northWall, &westWall, &southWall, &eastWall, bearing);*/
		
// 		set_ir_angle(LEFT, 45);
// 		set_ir_angle(RIGHT, -45);  
// 		sleep(1);
// 		get_front_ir_dists(&frontLeftReading, &frontRightReading);
		
		//////////////////////////////////////////////////
		
		maze[x][y] = (Sector){.northWall = northWall, .southWall = southWall, .eastWall = eastWall, .westWall = westWall, .visited = 1};
		
		//add previous location - the place we just left, with type = RETURN, instead of some north/south etc
		//DANGER add it at the END!!!, after everything else
		
		if(northWall == 0 && availSector(x, y+1, EXPLORING) == 1) pushBack(currentPath, (XY){.x = x, .y = y+1});
		if(southWall == 0 && availSector(x, y-1, EXPLORING) == 1) pushBack(currentPath, (XY){.x = x, .y = y-1});
		if(eastWall == 0 && availSector(x+1, y, EXPLORING) == 1) pushBack(currentPath, (XY){.x = x+1, .y = y});
		if(westWall == 0 && availSector(x-1, y, EXPLORING) == 1) pushBack(currentPath, (XY){.x = x-1, .y = y});
		
		if(northWall == 0 && availSector(x, y+1, northType) == 1) pushBack(currentPath, (XY){.x = x, .y = y+1});
		if(southWall == 0 && availSector(x, y-1, southType) == 1) pushBack(currentPath, (XY){.x = x, .y = y-1});
		if(eastWall == 0 && availSector(x+1, y, eastType) == 1) pushBack(currentPath, (XY){.x = x+1, .y = y});
		if(westWall == 0 && availSector(x-1, y, westType) == 1) pushBack(currentPath, (XY){.x = x-1, .y = y});
		
		printf("QUEUE FIRST ELEM: x %d y %d\n", currentPath->sxy.x, currentPath->sxy.y);
	}
}