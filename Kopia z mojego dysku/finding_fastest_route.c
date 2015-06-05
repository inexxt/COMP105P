#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DEBUG 0

#define WID 4
#define HEI 4
#define RES 60
#define SIZE_H HEI*RES
#define SIZE_W WID*RES

// #define SIZE SIZE_W //Only in case of square maze
#define SIZE 280

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define HORIZONTAL 1
#define VERTICAL 2

#define WALL 1

#define ZEROMOVE (move){0,0}
#define ZEROPOS (position){0,0}
#define STARTPOSITION (position){30, 0}

#define MAXSPEED SIZE/2 //are we able to 
//DANGER if the starting point (0,0) is in (0,-1) sector, then the code must be modified
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
            if(map[i][j] > 0) printf("%d", map[i][j]);
            else printf("%d", visited[i][j]);
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
//         printf("Entering push mode\n");
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

bool checksquare(double x, double y) //TODO -> note at the beginning
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
//     printf("target %d %d\n", target.x, target.y);
    if(previous.dx == 0 && previous.dy == 0 && next.dx == 0 && next.dy == 0) return 1;
    if(target.x >= SIZE || target.y>= SIZE || target.x < 0 || target.y < 0) return 2; //shouldn't occure
    // - the place itself (if it is available or not) or have been previously visited
    if(map[target.x][target.y]==WALL || visited[target.x][target.y]) return 3;
    // - the acceleration
    if(abs(previous.dx - next.dx) + abs(previous.dy - next.dy) > 1) return 4;
    //the maximal speed
    if(sqrt(next.dx*next.dx + next.dy*next.dy) > MAXSPEED) return 5;
    // - the route - if it intersects with any wall
    double length = next.dx*next.dx + next.dy*next.dy;
    double xpos = p.x;
    double ypos = p.y;
    double sinSlope = (double) (target.x - xpos)/length;
    double cosSlope = (double) (target.y - ypos)/length;
    
//     if(xpos > target.x) cosSlope = -cosSlope;
//     if(ypos > target.y) sinSlope = -sinSlope;
    
    while(abs(xpos-target.x)>0.5 || abs(ypos-target.y)>0.5)
    {
        //i'm moving every sin and cos, by doing so I can check every sector
        xpos += sinSlope;
        ypos += cosSlope;
//         printf("from %d %d to %d %d by %f %f\n", p.x, p.y, target.x, target.y, xpos, ypos);
        if (!checksquare(xpos, ypos)) return 6;
    }
    return 0;
}


bool isFinal(position sq) //TODO change it to check range, not single (x,y)
{
   return (sq.x > goal1.x) && (sq.y > goal1.y) && (sq.x < goal2.x) && (sq.y < goal2.y);
}


void setWall(int x1, int y1, int x2, int y2, int option) //for now - just normal walls
{
    int i;
    
    x1 += 30;
    x2 += 30;
    y1 += 60;
    y2 += 60;
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
    printf("XY %d %d %d %d\n", x1, y1, x2, y2);
    if (option == VERTICAL)
        for (i=y1; i<=y2; i++)
            map[x1][i] = WALL;
   
    if (option == HORIZONTAL)
        for (i=x1; i<=x2; i++)
            map[i][y1] = WALL;
}

void mapPreprocessing() //TODO map preprocessing - setting walls, setting walls' margins etc
{
    int i,j;
    int xcenter = 0;
    int ycenter = -RES;
    setWall(-1, -RES, -RES/2, -RES, HORIZONTAL);
    setWall(1, -RES, RES/2, -RES, HORIZONTAL);
    setWall(-RES/2, -RES, -RES/2, 0, VERTICAL);
    setWall(RES/2, -RES, RES/2, -RES/2, VERTICAL);
    
    for(i=0; i<WID; i++)
        for(j=0; j<HEI; j++)
        {
            int xcenter = maze[i][j].xCenter;
            int ycenter = maze[i][j].yCenter;
            if(maze[i][j].northWall)
                setWall(xcenter - RES/2, ycenter + RES/2, xcenter + RES/2, ycenter + RES/2, VERTICAL);
            if(maze[i][j].southWall)
                setWall(xcenter - RES/2, ycenter - RES/2, xcenter + RES/2, ycenter - RES/2, VERTICAL);
            if(maze[i][j].eastWall)
                setWall(xcenter + RES/2, ycenter - RES/2, xcenter + RES/2, ycenter + RES/2, HORIZONTAL);
            if(maze[i][j].westWall)
                setWall(xcenter - RES/2, ycenter - RES/2, xcenter - RES/2, ycenter + RES/2, HORIZONTAL);
        }
    goal1.x = (WID-1)*SIZE-SIZE/6;
    goal1.y = (HEI-1)*SIZE-SIZE/6;
    goal2.x = (WID-1)*SIZE+SIZE/6;
    goal2.y = (HEI-1)*SIZE+SIZE/6;
}

int abc = 1;

void printPath(position current) //DISCUSS 
{
    map[current.x][current.y] = (abc % 10);
    printf("%d %d %d\n", current.x, current.y, map[current.x][current.y]);
    if(current.x == 1 && current.y == 1) return;
    move prev = cameFrom[current.x][current.y];
    
    abc++;
    printPath((position){current.x - prev.dx, current.y - prev.dy});
}


move nextMove(move last, int option)
{
    move next = last;
    if(option == UP)
        next.dy += 1;
    if(option == DOWN)
        next.dy -= 1;
    if(option == RIGHT)
        next.dx += 1;
    if(option == LEFT)
        next.dx -= 1;
    
    return next;
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
        int i = 0;
        for(i=0; i<5; i++)
        {
            move next = nextMove(last, i);
            if(k==10)
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
                if(isFinal(newp)) return newp;

                if(BFSQueue != NULL)
                    append(BFSQueue, newp);
                else
                    BFSQueue = initializeQ(newp);
            }
        }
    }
    printf("ERROR: empty BFSQueue\n");
    return ZEROPOS;
}

positionQ* solve()
{
    mapPreprocessing();
//     printMap();            
    cameFrom[STARTPOSITION.x][STARTPOSITION.y] = (move){1,1};
    position finish = BFS(STARTPOSITION);
    printf("FINISH %d %d\n", finish.x, finish.y); 
    int k;
    scanf("%d", &k);
    printPath(finish);
    return NULL;
//     return makePointsQueue(finish); //I have full table of moves [where I came from], so i'm just going through from ending
}

int main()
{
    maze[0][0] = (Sector){0, 0, 1, 1, 1};
    maze[0][1] = (Sector){1, 0, 1, 0, 1};
    maze[0][2] = (Sector){0, 1, 1, 0, 1};
    maze[0][3] = (Sector){1, 0, 1, 1, 1};
    maze[1][0] = (Sector){1, 1, 1, 0, 1};
    maze[1][1] = (Sector){0, 1, 0, 0, 1};
    maze[1][2] = (Sector){1, 0, 0, 0, 1};
    maze[1][3] = (Sector){1, 1, 1, 0, 1};
    maze[2][0] = (Sector){1, 1, 0, 0, 1};
    maze[2][1] = (Sector){1, 1, 0, 0, 1};
    maze[2][2] = (Sector){0, 1, 0, 0, 1};
    maze[2][3] = (Sector){1, 0, 0, 0, 1};
    maze[3][0] = (Sector){0, 1, 0, 1, 1};
    maze[3][1] = (Sector){1, 0, 0, 1, 1};
    maze[3][2] = (Sector){1, 1, 0, 1, 1};
    maze[3][3] = (Sector){1, 1, 0, 1, 1};
    
    solve();
    printMapFinal();
    return 0;
}

