#include "findFastest.h"

#include "defines.h"
#include "phase1.h"
#include "phase1.map.h"
#include "phase1.h" //for maze array

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DEBUG 1

#define WID 4
#define HEI 4
#define RES 60
#define SIZE_H 280
#define SIZE_W 241

// #define SIZE SIZE_W //Only in case of square maze
#define SIZE 280

#define MAXNPATH 200 //maximal path length DANGER it may be not enough. Should be though, change only if path is VERY complicated

#define HORIZONTAL 1
#define VERTICAL 2

#define BIG 1
#define SMALL 0
#define THICKNESS 20

#define WALL 1

#define ZEROMOVE (move){0,0}
#define ZEROPOS (position){0,0}
#define STARTPOSITION (position){30, 1}

#define MAXSPEED 6 //max sped of the robot, approximately
#define SCALE 5

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

int map[SIZE][SIZE];
bool visited[SIZE][SIZE];
move cameFrom[SIZE][SIZE];
positionQ* BFSQueue;

position goal1, goal2;
position startingPoint;

position pathArray[MAXNPATH]; 

int pathLength;

Sector maze[WID][HEI];

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

bool isFinal(position sq) 
{
    double x = 210;
    double y = 240;
    if ((sq.x > x - ROBOT_WIDTH/3) && (sq.y > y - ROBOT_WIDTH/3) && (sq.x < x + ROBOT_WIDTH/3) && (sq.y < y + ROBOT_WIDTH/3)) return 1;  
    else return 0;
}

void printMapFinal()
{
    int i, j;
    for(j = SIZE - 1; j>=0; j--)
    {
        for(i = 0; i<SIZE; i++)
            if(map[i][j] > 0) printf("%d", map[i][j]);
            else printf(".");
        printf("\n");
    }
}


void printMapDebug()
{
    int i, j;
    for(j = SIZE - 1; j>=0; j--)
    {
        for(i = 0; i<SIZE; i++)
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


bool isQEmpty(positionQ * head)
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
    if(!isQEmpty(head))
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
        if (!checksquare(xpos, ypos)) return 6;
    }
    return 0;
}


void setWall(int x1, int y1, int x2, int y2, int option, int big) //big tells if the wall is thick
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
    for(j=x1-THICKNESS*big; j<=x1+THICKNESS*big; j+=1)
        for (i=y1-THICKNESS; i<=y2+THICKNESS; i++)
            if(x1 >= 0 && i >= 0 && j< SIZE && i<SIZE)
                map[j][i] = WALL;
   
    if (option == HORIZONTAL)
        for(j=y1-THICKNESS*big; j<=y1+THICKNESS*big; j+=1)    
            for (i=x1-THICKNESS; i<=x2+THICKNESS; i++)
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
    {
        for(j=0; j<HEI; j++)
        {
            xcenter = i*RES;
            ycenter = j*RES;
            
            int b = BIG;
            if(i == 3 && j == 3) b = SMALL; // for the final square forces its walls not to be thick
            if(maze[i][j].northWall)
                setWall(xcenter - RES/2, ycenter + RES/2, xcenter + RES/2, ycenter + RES/2, HORIZONTAL, b);
            if(maze[i][j].southWall)
                setWall(xcenter - RES/2, ycenter - RES/2, xcenter + RES/2, ycenter - RES/2, HORIZONTAL, b);
            if(maze[i][j].eastWall)
                setWall(xcenter + RES/2, ycenter - RES/2, xcenter + RES/2, ycenter + RES/2, VERTICAL, b);
            if(maze[i][j].westWall)
                setWall(xcenter - RES/2, ycenter - RES/2, xcenter - RES/2, ycenter + RES/2, VERTICAL, b);
        }
    }
    goal1.x = SIZE_W- 48;
    goal1.y = SIZE_H -60;
    goal2.x = SIZE_W-20;
    goal2.y = SIZE_H-22;
    if(DEBUG) printf("%d %d %d %d A\n", goal1.x, goal1.y, goal2.x, goal2.y);
    if(DEBUG) scanf("%d", &d);
    
}

int abc = 1;
int a;
void calculatePath(position current) //returns length of path, saves positions in pathArray
{
    if(DEBUG) printf("%d %d %d %d\n", current.x, current.y, STARTPOSITION.x, STARTPOSITION.y);
    
    pathArray[MAXNPATH - (++pathLength)] = current;
    move prev = cameFrom[current.x][current.y];
    if(current.x == STARTPOSITION.x && current.y == STARTPOSITION.y) return;
    if(DEBUG) map[current.x][current.y] = (abc % 10);
    if(DEBUG) abc++; // it allows printing final map with our path
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
    while(!isQEmpty(BFSQueue))
    {
        position p = BFSQueue->p;
        BFSQueue = BFSQueue->next;
        move last = cameFrom[p.x][p.y];
        k += DEBUG; // debug line
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
    clock_t t = clock();
           
    cameFrom[STARTPOSITION.x][STARTPOSITION.y] = (move){1,1};
    position finish = BFS(STARTPOSITION);
    printf("FINISH %d %d\n", finish.x, finish.y); 

    t = clock() - t;
    printf("Time taken in seconds for creating the path: %f\n", ((float)t)/CLOCKS_PER_SEC);
    calculatePath(finish);
    return;
}

void moveToTargetFastest(position p)
{
    double targetX = (double) p.x;
    double targetY = (double) p.y; //important to decrease by RES
    
    double remainingDistance = 1000.0;
    double previousRemainingDistance = 1000.0; 
    double xDifference, yDifference;

    double targetThreshold; 
    double requiredAngleChange;
    int baseSpeed;
    int leftSpeed = 0, rightSpeed = 0;

    
    xDifference = targetX - xPos;
    yDifference = targetY - yPos; 

    remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);
    
    
    targetThreshold = 4;
    baseSpeed = SCALE*remainingDistance;

    while (((fabs(remainingDistance)) > targetThreshold ) && (remainingDistance <= (previousRemainingDistance + targetThreshold)))
    {
        if(DEBUG) printf("I'm in %f %f, driving with the speed of %d, left: %d right: %d\n", xPos, yPos, baseSpeed, leftSpeed, rightSpeed);
        if(DEBUG) printf("\t Driving to %f %f , remaining distance %f \n", targetX, targetY, remainingDistance);    
        log_trail();
        updateRobotPosition(); 
        previousRemainingDistance = remainingDistance;
        
        xDifference = targetX - xPos;
        yDifference = targetY - yPos;  

        remainingDistance = sqrt(xDifference*xDifference + yDifference*yDifference);

        requiredAngleChange = atan2(xDifference,yDifference)- bearing; 

        while(requiredAngleChange > (M_PI))
        {
            requiredAngleChange -= (2 * M_PI);
        }
        while(requiredAngleChange < (-M_PI))
        {
            requiredAngleChange += (2 * M_PI);
        }

        leftSpeed = baseSpeed + baseSpeed * 2*requiredAngleChange;
        rightSpeed = baseSpeed - baseSpeed * 2*requiredAngleChange;

        if(fabs(requiredAngleChange) > (M_PI/2))
          break;
        set_motors(leftSpeed,rightSpeed);
    }
    set_motors(0,0);
    
}


void drive()
{
    int counter;
    if(DEBUG) printf("%d %d\n", MAXNPATH, pathLength);
    for(counter = MAXNPATH - pathLength + 1; counter < MAXNPATH; counter++)
    {
        
        if(DEBUG) printf("Counter %d value %d %d\n", counter, pathArray[counter].x, pathArray[counter].y);
        moveToTargetFastest(pathArray[counter]);
    }
    return;
}

int findFastest()
{
    updateRobotPosition();
    
    xPos += 30;
    yPos += 60;
    
    if(DEBUG) printf("Curreny position: %f %f, bearing: %f\n", xPos, yPos, bearing);  
    if(DEBUG) scanf("%d", &a);
    mapPreprocessing();
    printMapDebug(); 
    solve();
    printMapFinal();
    if(DEBUG)
    {
        int a;
        printf("Press any button: \n");
        scanf("%d", &a);
    }
    drive();
    return 0;
}

