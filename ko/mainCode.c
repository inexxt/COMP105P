#include "defines.h"
#include "phase1.map.h"
#include "phase1.move.h"
#include "phase2.h"
#include "recordPosition.h"
#include "basicMovement.h"

/*defined as extern globals*/
double targetWallReadings;
double sensorDifference;
Sector maze[MAZE_WIDTH][MAZE_HEIGHT];

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
	printf("------------------PRINTING MAZE---------------------\n");
	printf("----------------------------------------------------\n");
	int i,j;
	for(i = 0; i<4; i++)
		for(j = 0; j<4; j++)
		{
			printf("maze[%d][%d] = (Sector){%d, %d, %d, %d, %d, %f, %f};\n", i, j, maze[i][j].northWall, maze[i][j].southWall, maze[i][j].westWall, maze[i][j].eastWall, maze[i][j].visited, maze[i][j].xCenter, maze[i][j].yCenter);
		}
	printf("-------------------------------------------------\n");
	printf("-----------------ENDING MAZE---------------------\n");
}

void calibrateWallDistance()
{
	double frontLeftReading, frontRightReading, sideLeftReading, sideRightReading; 
	double frontLeftReading2, frontRightReading2, sideLeftReading2, sideRightReading2; 
	set_ir_angle(LEFT, -45);
  	set_ir_angle(RIGHT, 45);    
  	usleep(SLEEPTIME);
	getFrontIR(&frontLeftReading, &frontRightReading);
	getSideIR(&sideLeftReading, &sideRightReading);
	usleep(SLEEPTIME);
	getFrontIR(&frontLeftReading2, &frontRightReading2);
	getSideIR(&sideLeftReading2, &sideRightReading2);

	targetWallReadings = (frontLeftReading + frontRightReading + sideLeftReading + sideRightReading 
				+ frontLeftReading2 + frontRightReading2 + sideLeftReading2 + sideRightReading2)/8.0;
	sensorDifference = ((frontRightReading + frontRightReading2 + frontLeftReading + frontLeftReading2) 
				- (sideRightReading + sideRightReading2 + sideLeftReading + sideLeftReading2))/4.0;
}

int main() 
{
	connect_to_robot();
	initialize_robot();
	printf("Starting mapping...\n");

	//PHASE 1
	calibrateWallDistance();

	Queue* currentPath = NULL;
	XY current;
	XY first = {.x = 0, .y = 0};
	maze[0][0].visited = 2;
    pushFront(&currentPath, first);
    updateRobotPosition();
    xPos = 0;
	yPos = -SECTOR_WIDTH;

   
	while(!(current.y == -1 && current.x == 0))
	{
		printf("Current position: x: %f, y: %f\n", xPos, yPos);
		current = nextSector(&currentPath);
		goToXY(current, 1);
		updateSector(&currentPath);
	}
	printfMaze();
	endPhase1();
    
	//PHASE 2
	//Solution 1
	Queue* a = calculateOptimalPath();
	while(!isEmpty(a))
	{
		goToXY(popFront(&a), 2);
	}
   
    //Solution 2
    // go(); 
	
    set_motors(0,0);
	sleep(1);
	printf("FINISH\n");
	return 0;
}





// 	maze[0][0] = (Sector){0, 0, 1, 1, 1};
// 	maze[0][1] = (Sector){1, 0, 1, 0, 1};
// 	maze[0][2] = (Sector){0, 1, 1, 0, 1};
// 	maze[0][3] = (Sector){1, 0, 1, 1, 1};
// 	maze[1][0] = (Sector){1, 1, 1, 0, 1};
// 	maze[1][1] = (Sector){0, 1, 0, 0, 1};
// 	maze[1][2] = (Sector){1, 0, 0, 0, 1};
// 	maze[1][3] = (Sector){1, 1, 1, 0, 1};
// 	maze[2][0] = (Sector){1, 1, 0, 0, 1};
// 	maze[2][1] = (Sector){1, 1, 0, 0, 1};
// 	maze[2][2] = (Sector){0, 1, 0, 0, 1};
// 	maze[2][3] = (Sector){1, 0, 0, 0, 1};
// 	maze[3][0] = (Sector){0, 1, 0, 1, 1};
// 	maze[3][1] = (Sector){1, 0, 0, 1, 1};
// 	maze[3][2] = (Sector){1, 1, 0, 1, 1};
// 	maze[3][3] = (Sector){1, 1, 0, 1, 1};
