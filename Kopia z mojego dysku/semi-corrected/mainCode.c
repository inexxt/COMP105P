#include "defines.h"
#include "phase1.map.h"
#include "phase1.move.h"
#include "phase2.h"
#include "recordPosition.h"
#include "basicMovement.h"


#define CUTTING 1

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
	double frontLeftReading = 0;
	double frontRightReading = 0;
	double sideLeftReading = 0;
	double sideRightReading = 0; 
	double frontLeftReading2 = 0;
	double frontRightReading2 = 0;
	double sideLeftReading2 = 0; 
	double sideRightReading2 = 0; 
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

XY qP[100];
int length = 100;

void printfMyQueue()
{
    int j=0;
    printf("length %d\n", length);
    for(j = 0; j<length; j++)
    {

            printf("qP[%d] : x %d y %d\n", j, qP[j].x, qP[j].y);
    }
}

void putInN(int n, XY p)
{
    printf("Putting %d : x %d y %d\n", n, p.x, p.y);
    int j = 0;
    for(j = length-1; j>n; j--)
    {
        qP[j] = qP[j-1];

    }
    qP[n] = p;

    length++;
}

void deleteFromN(int n)
{
    int j = 0;
    printf("Deleting %d : x %d y %d\n", j, qP[n].x, qP[n].y);
    for(j = n; j<length; j++)
    {
        qP[j] = qP[j+1];

    }
    length--;
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
   
//     maze[0][0] = (Sector){1, 0, 1, 0, 1, 0.479304, -8.030782};
//     maze[0][1] = (Sector){0, 1, 1, 0, 1, 0.479304, 51.573389};
//     maze[0][2] = (Sector){0, 0, 1, 1, 1, 0.479304, 112.500000};
//     maze[0][3] = (Sector){1, 0, 1, 0, 1, 0.479304, 171.700673};
//     maze[1][0] = (Sector){0, 1, 0, 0, 1, 59.581096, -8.030782};
//     maze[1][1] = (Sector){0, 0, 0, 1, 1, 59.581096, 51.573389};
//     maze[1][2] = (Sector){1, 0, 1, 0, 1, 59.581096, 112.500000};
//     maze[1][3] = (Sector){1, 1, 0, 0, 1, 59.581096, 171.700673};
//     maze[2][0] = (Sector){1, 1, 0, 0, 1, 119.542941, -8.030782};
//     maze[2][1] = (Sector){0, 1, 1, 1, 1, 119.542941, 51.573389};
//     maze[2][2] = (Sector){0, 0, 0, 1, 1, 119.542941, 112.500000};
//     maze[2][3] = (Sector){1, 0, 0, 0, 1, 119.542941, 171.700673};
//     maze[3][0] = (Sector){0, 1, 0, 1, 1, 178.625845, -8.030782};
//     maze[3][1] = (Sector){0, 0, 1, 1, 1, 178.625845, 51.573389};
//     maze[3][2] = (Sector){1, 0, 1, 1, 1, 178.625845, 112.500000};
//     maze[3][3] = (Sector){1, 1, 0, 1, 1, 178.625845, 171.700673};

	//PHASE 2
	//Solution 1
	Queue* a = calculateOptimalPath();
   
//     optimizeQueue(); //DANGER test it!!

   
   
//    ------------------------- CUTTING
    if(CUTTING)
    {
        
        int i = 0;
        for(i=0; i<length; i++)
        {
            qP[i] = (XY){0, 0};
        }
        
        length = 0;
        XY help;
        while(!isEmpty(a))
        {
            help = popFront(&a);
            qP[length] = (XY){.x = help.x*60, help.y*60}; 
            length++;
        }
        
        int px, py, cx, cy, ax, ay, bx, by;

        printfMyQueue();
        
        int k = 1;
        
        XY b1, b2;
        
        length++;
        
        for(k=0; k<length - 2; k++)
        {
            ax = qP[k].x;
            ay = qP[k].y;
            
            cx = qP[k+2].x;
            cy = qP[k+2].y;
            
            printf("first %d %d twoAfter %d %d difference %d %d\n", ax, ay, cx, cy, abs(ax - cx), abs(ay - cy));
            if(abs(ax - cx) == 60 && abs(ay - cy) == 60 && ax % 60 == 0 && ay % 60 == 0 && cy % 60 == 0 && cx % 60 == 0) //we can cut corner
            {
                px = qP[k+1].x;
                py = qP[k+1].y;
                
                deleteFromN(k+1);
                
                bx = (ax+cx)/2;
                by = (ay+cy)/2;
                
                if(py != ax)
                {
                    b1.x = ax;
                    b1.y = by;
                    
                    b2.x = bx;
                    b2.y = cy;
                }
                else
                {
                    
                    b1.x = bx;
                    b1.y = ay;
            
                    b2.x = cx;
                    b2.y = by;
                }
                b1.x += (b1.x-bx)/3;
                b1.y += (b1.y-by)/3;
            
                b2.x += (b2.x-bx)/3;
                b2.y += (b2.y-by)/3;
                    
                putInN(k+1, b2);
                putInN(k+1, b1);
            }
        }
        
        length--;
        
        printfMyQueue();
        
        for(i=0; i<=length; i++)
        {
            if(qP[i].y != 0 || i < 2)
            {
                rotateTowardsTarget(qP[i].x, qP[i].y, 2);
                moveToTarget(qP[i].x, qP[i].y, 2);
            }
        }
    }
    //    ------------------------- CUTTING
    
    
    
    
//    ------------------------- NO CUTTING 
    if(!CUTTING)
    {
        while(!isEmpty(a))
        {
            goToXY(popFront(&a), 2);
        }
    }

//    ------------------------- NO CUTTING

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
