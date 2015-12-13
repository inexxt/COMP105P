#include "findFastest.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DEBUG 0

#define WID 4
#define HEI 4
#define RES 60
#define SIZE_H 280
#define SIZE_W 241

// #define SIZE SIZE_W //Only in case of square maze
#define SIZE 280

#define MAXNPATH 200 //maximal path length DANGER it may be not enough. Should be though, change only if path is VERY complicated
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define HORIZONTAL 1
#define VERTICAL 2

#define BIG 1
#define SMALL 0

#define WALL 1

#define ZEROMOVE (move){0,0}
#define ZEROPOS (position){0,0}
#define STARTPOSITION (position){30, 0}

#define MAXSPEED RES //max sped of the robot, approximately
#define SCALE (1.0/60)
//DANGER TODO think about the situation when I'm not exactly at the center of square, or have some slip or anything like that

typedef struct
{
   int dx;
   int dy;
} move;

typedef struct
{
    int x;
    int y;
} position;

typedef struct positionQ_t
{
   position p;
   struct positionQ_t * next;
} positionQ;

typedef struct
{
        int northWall;
        int southWall;
        int westWall;
        int eastWall;
        int visited;

        int xCenter;
        int yCenter;

} Sector;

Sector maze[WID][HEI];


// position center[WID][HEI]; //for each sector it contains the position of it's center, supplied by (map phase)
int map[SIZE][SIZE];
bool visited[SIZE][SIZE];
move cameFrom[SIZE][SIZE];
positionQ* BFSQueue;

position goal1, goal2;
position startingPoint;

position pathArray[MAXNPATH]; 

int pathLength;

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b; 
    *b = t;
}

void swapd(double* a, double* b)
{
    double t = *a;
    *a = *b; 
    *b = t;
}

bool isFinal(position sq) //TODO change it to check range, not single (x,y)
{
   if ((sq.x > goal1.x) && (sq.y > goal1.y) && (sq.x < goal2.x) && (sq.y < goal2.y)) return 1;
   else return 0;
}

void printMapFinal()
{
    int i, j;
    for(i = 0; i<SIZE; i++)
    {
        for(j = 0; j<SIZE; j++)
            if(map[i][j] > 0) printf("%d", map[i][j]);
            else printf(".");
        printf("\n");
    }
}


void printMapDebug()
{
    int i, j;
    for(i = 0; i<SIZE; i++)
    {
        for(j = 0; j<SIZE; j++)
        {   
            if(isFinal((position){i, j})) printf("█");
                else
                if(map[i][j] > 0) printf("%d", map[i][j]);
                    else 
                    if(visited[i][j]) printf("."); 
                    else printf(" ");
        }
        printf("\n");
    }
}


bool isEmpty(positionQ * head)
{
    if(head == NULL) return true;
    return false;
}

positionQ* initializeQ(position p)
{
   positionQ * head = NULL;
   head = malloc(sizeof(positionQ));
   head->p = p;
   head->next = NULL;
   return head;
}

position lastPosition(positionQ * head)
{
    positionQ * current = head;
    while (current->next != NULL) 
    {
        current = current->next;
    }
    return current->p;
}

int lengthQ(positionQ * head)
{
    positionQ * current = head;
    int len = 0;
    if(!isEmpty(head))
        while (current->next != NULL) 
        {
            current = current->next;
            len++;
        }
    return len;
}

void append(positionQ * head, position p) 
{
    positionQ * current = head;
    if(current != NULL)
    {
        while (current->next != NULL) 
        {
            current = current->next;
        }
//         if(DEBUG) printf("Entering push mode\n");
        current->next = malloc(sizeof(positionQ));
        
        current->next->p = p;
        current->next->next = NULL;
    }
}



position nextPosition(position p, move m)
{
    return (position){p.x + m.dx, p.y + m.dy};
}

position previousPosition(position p, move m)
{
    return (position){p.x - m.dx, p.y - m.dy};
}

bool checksquare(double x, double y) //just checks the square where (x,y) belongs
{
    int xi = (int)floor(x);
    int yi = (int)floor(y);
    
    if(map[xi][yi] == WALL) return false;
    return true;
}

int isLegal(move previous, position p, move next)
{
    position target = nextPosition(p, next);
    //to build the graph
    
    //checking if I can go that way:
    if(previous.dx == 0 && previous.dy == 0 && next.dx == 0 && next.dy == 0) return 1;
    if(target.x >= SIZE || target.y>= SIZE || target.x < 0 || target.y < 0) return 2; //shouldn't occure
    
    // - the place itself (if it is available or not) or have been previously visited
    if(map[target.x][target.y]==WALL || visited[target.x][target.y]) return 3;
    
    // - the acceleration
    if(abs(previous.dx - next.dx) + abs(previous.dy - next.dy) > 2) return 4;
    
    //the maximal speed
    if(sqrt(next.dx*next.dx + next.dy*next.dy) > MAXSPEED) return 5;
   
    // - the route - if it intersects with any wall
    double length = next.dx*next.dx + next.dy*next.dy;
    double xpos = p.x;
    double ypos = p.y;
    double sinSlope = (double) (target.x - xpos)/length;
    double cosSlope = (double) (target.y - ypos)/length;

    while(abs(xpos-target.x)>0.5 || abs(ypos-target.y)>0.5)
    {
        //i'm moving every sin and cos, by doing so I can check every sector
        xpos += sinSlope;
        ypos += cosSlope;
//         if(DEBUG) printf("from %d %d to %d %d by %f %f\n", p.x, p.y, target.x, target.y, xpos, ypos);
        if (!checksquare(xpos, ypos)) return 6;
    }
    return 0;
}


void setWall(int x1, int y1, int x2, int y2, int option, int big) //big means thick
{
    int i, j;
    
    x1 += RES/2;
    x2 += RES/2;
    y1 += RES;
    y2 += RES;
    if(x1 > x2) 
    {
        int t = x1;
        x1 = x2;
        x2 = t;
    }
    if(x1 > x2) 
    {
        int t = y1;
        y1 = y2;
        y2 = t;
    }
    if(DEBUG) printf("XY %d %d %d %d\n", x1, y1, x2, y2);
    if (option == VERTICAL)
    for(j=x1-12*big; j<=x1+12*big; j+=1)
        for (i=y1; i<=y2; i++)
            if(x1 >= 0 && i >= 0 && j< SIZE && i<SIZE)
                map[j][i] = WALL;
   
    if (option == HORIZONTAL)
        for(j=y1-12*big; j<=y1+12*big; j+=1)    
            for (i=x1; i<=x2; i++)
                if(j >= 0 && i >= 0 && j< SIZE && i<SIZE)
                    map[i][j] = WALL;
}

void mapPreprocessing()
{
    int i,j;
    int xcenter = 0;
    int ycenter = -RES;
    setWall(-RES/2, -RES, -RES/2, -RES/2, VERTICAL, SMALL);
    setWall(-RES/2, -RES, -1, -RES, HORIZONTAL, SMALL);
    setWall(1, -RES, RES/2, -RES, HORIZONTAL, SMALL);
    setWall(RES/2, -RES, RES/2, -RES/2, VERTICAL, SMALL);
    int d;
    
    for(i=0; i<WID; i++)
        for(j=0; j<HEI; j++)
        {
            
//             int xcenter = maze[i][j].xCenter;
//             int ycenter = maze[i][j].yCenter;
            int xcenter = i*RES;
            int ycenter = j*RES;
            
            if(maze[i][j].northWall)
                setWall(xcenter - RES/2, ycenter + RES/2, xcenter + RES/2, ycenter + RES/2, HORIZONTAL, BIG);
            if(maze[i][j].southWall)
                setWall(xcenter - RES/2, ycenter - RES/2, xcenter + RES/2, ycenter - RES/2, HORIZONTAL, BIG);
            if(maze[i][j].eastWall)
                setWall(xcenter + RES/2, ycenter - RES/2, xcenter + RES/2, ycenter + RES/2, VERTICAL, BIG);
            if(maze[i][j].westWall)
                setWall(xcenter - RES/2, ycenter - RES/2, xcenter - RES/2, ycenter + RES/2, VERTICAL, BIG);
//             printMapDebug();
//             scanf("%d", &d);
        }
    goal1.x = SIZE_W- 48;
    goal1.y = SIZE_H -55;
    goal2.x = SIZE_W-20;
    goal2.y = SIZE_H-25;
    if(DEBUG) printf("%d %d %d %d A\n", goal1.x, goal1.y, goal2.x, goal2.y);
    if(DEBUG) scanf("%d", &d);
    
}

int abc = 1;
int a;
void calculatePath(position current) //returns length of path, saves positions in pathArray
{
    map[current.x][current.y] = (abc % 10);
    if(DEBUG) printf("%d %d %d %d\n", current.x, current.y, STARTPOSITION.x, STARTPOSITION.y);
    
    pathArray[MAXNPATH - (++pathLength)] = current;
    move prev = cameFrom[current.x][current.y];
    if(current.x == STARTPOSITION.x && current.y == STARTPOSITION.y) return;
    abc++;
    calculatePath((position){current.x - prev.dx, current.y - prev.dy});
}


move nextMove(move next, int i, int j)
{   
    return (move){next.dx + i - 2, next.dy + j - 2};
}


position BFS(position p)
{
    int k = 0, xs;
    BFSQueue = malloc(sizeof(positionQ));
    BFSQueue->p = p;
    BFSQueue->next = NULL;
    
    visited[p.x][p.y] = 1;
    while(!isEmpty(BFSQueue))
    {
        position p = BFSQueue->p;
        BFSQueue = BFSQueue->next;
        move last = cameFrom[p.x][p.y];
        
        k += DEBUG; //DEBUG LINE
        int i, j;
        for(i=0; i<5; i++)
        {
            for(j = 0; j<4; j++)
            {
                move next = nextMove(last, i, j);
                if(k==1000)
                {
                    k=0;
                    printf("isLegal %d %d %d %d %d\n", p.x, p.y, next.dx, next.dy, isLegal(last, p, next));
                    printMapDebug();
                    scanf("%d", &xs);
                    
                }
                if(isLegal(last, p, next) == 0)
                {
                    position newp = nextPosition(p, next);  
                    cameFrom[newp.x][newp.y] = next;
                    visited[newp.x][newp.y] = 1;
    //                 printf("APPENDING %d %d\n", next.dx, next.dy);
                    if(isFinal(newp))
                    {
                        if(DEBUG) printf("FINAL %d %d\n", newp.x, newp.y);
                        return newp;
                    }
                    if(BFSQueue != NULL)
                        append(BFSQueue, newp);
                    else
                        BFSQueue = initializeQ(newp);
                }
            }
        }
    }
    printf("ERROR: empty BFSQueue\n");
    return STARTPOSITION;
}

void solve() //returns length of path
{
    int k = 1;
    clock_t t = clock();
           
    cameFrom[STARTPOSITION.x][STARTPOSITION.y] = (move){1,1};
    position finish = BFS(STARTPOSITION);
    printf("FINISH %d %d\n", finish.x, finish.y); 

    t = clock() - t;
    printf("Time taken in seconds: %f\n", ((float)t)/CLOCKS_PER_SEC);
    calculatePath(finish);
    return;
//     return makePointsQueue(finish); //I have full table of moves [where I came from], so i'm just going through from ending
}

void moveToTarget(position p)
{
    double targetX = p.x;
    double targetY = p.y - RES/2; //important to decrease by RES/2
    
    printf("I'm in %f %f\n", targetX, targetY);
/*    
    
    double remainingDistance;
    double previousRemainingDistance; 
    double xDifference, yDifference;

    double targetThreshold; 
    double requiredAngleChange;
    int baseSpeed;
    int leftSpeed, rightSpeed;

    
    xDifference = targetX - xPos; //TODO xPos
    yDifference = targetY - yPos; //TODO yPos 

    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);
    
    
    targetThreshold = 8;
    baseSpeed = SCALE*remainingDistance;

    while (((fabs(remainingDistance)) > targetThreshold ) && (remainingDistance <= (previousRemainingDistance + targetThreshold)))
    {
//         bumpers();
//         log_trail();
//         updateRobotPosition(); 
        previousRemainingDistance = remainingDistance;

        requiredAngleChange = atan2(xDifference,yDifference)- bearing; //TODO bearing

        while(requiredAngleChange > (M_PI))
        {
            requiredAngleChange -= (2 * M_PI);
        }
        while(requiredAngleChange < (-M_PI))
        {
            requiredAngleChange += (2 * M_PI);
        }

        leftSpeed = baseSpeed + baseSpeed * requiredAngleChange;
        rightSpeed = baseSpeed - baseSpeed * requiredAngleChange;

        if(fabs(requiredAngleChange) > (M_PI/4))
            break;

//         set_motors(leftSpeed,rightSpeed);
    }
//     set_motors(0,0);*/
    
}


void drive()
{
    int counter;
    printf("%d %d\n", MAXNPATH, pathLength);
    for(counter = MAXNPATH - pathLength; counter < MAXNPATH; counter++)
    {
        moveToTarget(pathArray[counter]);
    }
    return;
}

int mainFunction()
{
//     maze[0][0] = (Sector){0, 0, 1, 1, 1, 0, 0};
//     maze[0][1] = (Sector){1, 0, 1, 0, 1, 0, 60};
//     maze[0][2] = (Sector){0, 1, 1, 0, 1, 0, 120};
//     maze[0][3] = (Sector){1, 0, 1, 1, 1, 0, 180};
//     maze[1][0] = (Sector){1, 1, 1, 0, 1, 60, 0};
//     maze[1][1] = (Sector){0, 1, 0, 0, 1, 60, 60};
//     maze[1][2] = (Sector){1, 0, 0, 0, 1, 60, 120};
//     maze[1][3] = (Sector){1, 1, 1, 0, 1, 60, 180};
//     maze[2][0] = (Sector){1, 1, 0, 0, 1, 120, 0};
//     maze[2][1] = (Sector){1, 1, 0, 0, 1, 120, 60};
//     maze[2][2] = (Sector){0, 1, 0, 0, 1, 120, 120};
//     maze[2][3] = (Sector){1, 0, 0, 0, 1, 120, 180};
//     maze[3][0] = (Sector){0, 1, 0, 1, 1, 180, 0};
//     maze[3][1] = (Sector){1, 0, 0, 1, 1, 180, 60};
//     maze[3][2] = (Sector){1, 1, 0, 1, 1, 180, 120};
//     maze[3][3] = (Sector){1, 1, 0, 1, 1, 180, 180};
    
//     maze[0][0] = (Sector){0, 0, 1, 0, 1};
//     maze[0][1] = (Sector){1, 0, 1, 0, 1};
//     maze[0][2] = (Sector){0, 1, 1, 0, 1};
//     maze[0][3] = (Sector){1, 0, 1, 0, 1};
//     maze[1][0] = (Sector){1, 1, 0, 0, 1};
//     maze[1][1] = (Sector){0, 1, 0, 1, 1};
//     maze[1][2] = (Sector){1, 0, 0, 0, 1};
//     maze[1][3] = (Sector){1, 1, 0, 0, 1};
//     maze[2][0] = (Sector){1, 1, 0, 0, 1};
//     maze[2][1] = (Sector){0, 1, 1, 0, 1};
//     maze[2][2] = (Sector){1, 0, 0, 0, 1};
//     maze[2][3] = (Sector){1, 1, 0, 1, 1};
//     maze[3][0] = (Sector){0, 1, 0, 1, 1};
//     maze[3][1] = (Sector){1, 0, 0, 0, 1};
//     maze[3][2] = (Sector){0, 1, 0, 1, 1};
//     maze[3][3] = (Sector){1, 0, 1, 0, 1};
    
//     maze[0][0] = (Sector){0, 0, 1, 0, 1};
//     maze[0][1] = (Sector){0, 0, 1, 0, 1};
//     maze[0][2] = (Sector){1, 0, 1, 0, 1};
//     maze[0][3] = (Sector){1, 1, 1, 0, 1};
//     maze[1][0] = (Sector){1, 1, 0, 1, 1};
//     maze[1][1] = (Sector){1, 1, 0, 0, 1};
//     maze[1][2] = (Sector){0, 1, 0, 1, 1};
//     maze[1][3] = (Sector){1, 0, 0, 0, 1};
//     maze[2][0] = (Sector){0, 1, 1, 0, 1};
//     maze[2][1] = (Sector){0, 0, 0, 1, 1};
//     maze[2][2] = (Sector){0, 0, 1, 0, 1};
//     maze[2][3] = (Sector){1, 0, 0, 0, 1};
//     maze[3][0] = (Sector){1, 1, 0, 1, 1};
//     maze[3][1] = (Sector){0, 1, 1, 1, 1};
//     maze[3][2] = (Sector){0, 0, 0, 1, 1};
//     maze[3][3] = (Sector){1, 0, 1, 1, 1};
    
//     maze[0][0] = (Sector){1, 0, 1, 0, 1};
//     maze[0][1] = (Sector){0, 1, 1, 0, 1};
//     maze[0][2] = (Sector){0, 0, 1, 1, 1};
//     maze[0][3] = (Sector){1, 0, 1, 0, 1};
//     maze[1][0] = (Sector){0, 1, 0, 0, 1};
//     maze[1][1] = (Sector){0, 0, 0, 1, 1};
//     maze[1][2] = (Sector){1, 0, 1, 0, 1};
//     maze[1][3] = (Sector){1, 1, 0, 0, 1};
//     maze[2][0] = (Sector){1, 1, 0, 0, 1};
//     maze[2][1] = (Sector){0, 1, 1, 1, 1};
//     maze[2][2] = (Sector){0, 0, 0, 1, 1};
//     maze[2][3] = (Sector){1, 0, 0, 0, 1};
//     maze[3][0] = (Sector){0, 0, 0, 0, 1};
//     maze[3][1] = (Sector){0, 0, 1, 0, 1};
//     maze[3][2] = (Sector){1, 0, 1, 0, 1};
//     maze[3][3] = (Sector){1, 1, 0, 1, 1};

    mapPreprocessing();
    printMapDebug(); 
    solve();
    printMapFinal();
    drive();
    return 0;
}

