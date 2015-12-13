#include "mainCode.h"

#define MODE 3

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
            printf("maze[%d][%d] = (Sector){%d, %d, %d, %d, %d, %f, %f};\n", i, j, maze[i][j].northWall, maze[i][j].southWall, maze[i][j].westWall, maze[i][j].eastWall, maze[i][j].visited, maze[i][j].xCenter, maze[i][j].yCenter);
        
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

int main() 
{
    connect_to_robot();
    initialize_robot();
    printf("Starting mapping...\n");

    //BEGINNING PHASE 1
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
    //END PHASE 1
    
    //BEGINNING PHASE 2
    Queue* a = calculateShortestPath();
    //optimizeQueue(); //may optimize queue for no_cutting, but not tested
    
    if(MODE == 1) //solution with cutting corners
    {
        goWithCutting(&a);
    }

    if(MODE == 2) //solution with no cutting corners
    {
        while(!isEmpty(a))
        {
            goToXY(popFront(&a), 2);
        }
    }
    
    if(MODE == 3)
    {
        goOptimal(); //solution with _optimal_ path
    }

    set_motors(0,0);
    sleep(1);
    //END PHASE 2
    
    printf("FINISH\n");
    return 0;
}