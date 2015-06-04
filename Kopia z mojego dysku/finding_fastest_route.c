#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define WID 4
#define HEI 4
#define RES 20
#define SIZE_H HEI*RES
#define SIZE_W WID*RES

// #define SIZE SIZE_W //Only in case of square maze
#define SIZE 241

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define HORIZONTAL 1
#define VERTICAL 2

#define WALL 1

#define ZEROMOVE (move){0,0}
#define ZEROPOS (position){0,0}

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

void printMap()
{
    int i, j;
    for(i = 0; i<SIZE; i++)
    {
        for(j = 0; j<SIZE; j++)
            if(map[i][j] > 0) printf("%d", map[i][j]);
            else printf(".");
//             else printf("%d", visited[i][j]);
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
    double sinSlope = (double) next.dx/length;
    double cosSlope = (double) next.dy/length;
    
    if(xpos > target.x) cosSlope = -cosSlope;
    if(ypos > target.y) sinSlope = -sinSlope;
    
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
   return (sq.x == goalx) && (sq.y == goaly);
}


void setWall(int x1, int y1, int x2, int y2, int option) //for now - just normal walls
{
    int i;
    if(x1 > x2) swap(&x1, &x2);
    if(y1 > y2) swap(&y1, &y2);
    
//     printf("%d %d %d %d\n", x1, x2, y1, y2);
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
    xcenter = 0;
    ycenter = -RES;
    setWall(-1, -RES, -RES/2, -RES, HORIZONTAL);
    setWall(1, -RES, RES/2, -RES, HORIZONTAL);
    setWall(-RES/2, -RES, -RES/2, 0, VERTICAL);
    setWall(RES/2, -RES, RES/2, -RES/2, VERTICAL);
    
    for(i=0; i<WID; i++)
        for(j=0; j<HEI; j++)
        {
            int xcenter = maze[x][y].xCenter;
            int ycenter = maze[x][y].yCenter;
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
        
//         k++;
        int i = 0;
        for(i=0; i<5; i++)
        {
            move next = nextMove(last, i);
//             if(k==1)
//             {
//                 k=0;
//                 printf("isLegal %d %d %d %d %d\n", p.x, p.y, next.dx, next.dy, isLegal(last, p, next));
//                 printMap();
//                 scanf("%d", &xs);
//                 
//             }
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
    printf("ERROR: empty BFSQueue");
    return ZEROPOS;
}

positionQ* solve()
{
    mapPreprocessing();
//     printMap();            
    cameFrom[ZEROPOS.x+1][ZEROPOS.y +1] = (move){1,1};
    position finish = BFS((position){1,1});
    printf("%d %d\n", finish.x, finish.y); 
    printPath(finish);
    return NULL;
//     return makePointsQueue(finish); //I have full table of moves [where I came from], so i'm just going through from ending
}

int main()
{
    solve();
    printMap();
    return 0;
}

