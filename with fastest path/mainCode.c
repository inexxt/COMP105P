#include "defines.h"
#include "phase1.map.h"
#include "phase1.move.h"
#include "phase2.h"

void debug()
{
	printCurrentSector();
}

void printfSector(XY s)
{
	printf("sector x %d y%d", s.x, s.y);
}

void printfMaze()
{
	int i,j;
	for(i = 0; i<4; i++)
		for(j = 0; j<4; j++)
		{
			printf("maze[%d][%d] = (Sector){%d, %d, %d, %d, %d};\n", i, j, maze[i][j].northWall, maze[i][j].southWall, maze[i][j].westWall, maze[i][j].eastWall, maze[i][j].visited);
		}
}

int main() 
{
	connect_to_robot();
	initialize_robot();
// 	printf("beginning\n");
// 	////////////////////////// phase1
// 	Queue* currentPath = NULL;
// 	XY current;
// 
// 	XY first = {.x = 0, .y = 0};
// 	maze[0][0].visited = 2;
//     pushFront(&currentPath, first);
// 
	centerStartingPosition();
// 	int setOrigin = 1; // so it wouldnt try to set origin again
// 	while(current.y != -1 && current.x != -1)
// 	{
// // 		printf("BEING IN %d %d\n", current.x, current.y);
// // 		printQueue(currentPath);
// 		current = nextSector(&currentPath);
// // 		printfSector(current);
// 		printf("about to go to XY\n");
// 		goToXY(current);
// 		printf("about to updateSector\n");
// 		updateSector(&currentPath);
// 		printf("about to correctPosition\n");
// 		correctPosition(current);
// 		if((current.x == 0 && current.y == 0) && (setOrigin))
// 		{
// 			set_origin();
// 			setOrigin = 0;
// 		}
// 	}
// 		
// 	endPhase1();
// 	printfMaze();
// // 	Queue* optimalPath = calculateOptimalPath();
// // 	followPath(optimalPath);
// 	
// 	set_motors(0,0);
// 	printf("FINISH\n");
	maze[0][0] = (Sector){0, 0, 1, 1, 1};
	maze[0][1] = (Sector){0, 0, 1, 0, 1};
	maze[0][2] = (Sector){0, 0, 1, 0, 1};
	maze[0][3] = (Sector){1, 0, 1, 1, 1};
	maze[1][0] = (Sector){0, 1, 1, 0, 1};
	maze[1][1] = (Sector){1, 0, 0, 0, 1};
	maze[1][2] = (Sector){0, 1, 0, 1, 1};
	maze[1][3] = (Sector){1, 0, 1, 0, 1};
	maze[2][0] = (Sector){1, 1, 0, 0, 1};
	maze[2][1] = (Sector){0, 1, 0, 1, 1};
	maze[2][2] = (Sector){0, 0, 1, 0, 1};
	maze[2][3] = (Sector){1, 0, 0, 0, 1};
	maze[3][0] = (Sector){0, 1, 0, 1, 1};
	maze[3][1] = (Sector){0, 0, 1, 1, 1};
	maze[3][2] = (Sector){1, 0, 0, 1, 1};
	maze[3][3] = (Sector){1, 1, 0, 1, 1};

	Queue* a = calculateOptimalPath();
	
	while(!isEmpty(a))
	{
		goToXY(popFront(&a));
	}
	
	/////////////////////////// 
	
	return 0;
}