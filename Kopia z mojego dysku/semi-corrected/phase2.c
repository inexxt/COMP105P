#include "phase2.h"

typedef struct
{
	XY act;
	int prev;
} Elem;

int visited[4][4]; //1 - visited, 2 - added to q
int checker = 1;

int available(XY s, XY sp, int dir)
{	
    if(checker)
    {
        int x = s.x;
        int y = s.y;
        int xp = sp.x;
        int yp = sp.y;
        if(x > MAZE_WIDTH - 1 || x < 0 || y > (MAZE_HEIGHT - 1) || y < 0) return 0;
        if(dir == 1 && maze[xp][yp].northWall != 0) return 0;
        if(dir == 2 && maze[xp][yp].southWall != 0) return 0;
        if(dir == 3 && maze[xp][yp].westWall != 0) return 0;
        if(dir == 4 && maze[xp][yp].eastWall != 0) return 0;
        if(visited[x][y] != 0) return 0;
        printf("Adding (%d %d) from (%d %d)\n", x,y,xp,yp);
        if(x == 3 && y == 3) checker = 0;
        return 1;
    }
    else 
        return 0;
}

Queue* calculateShortestPath()
{
    Queue* oP = NULL;
    XY cur = {.x = 0, .y = 0};
    
    int i, j;
    for(i = 0; i<4; i++)
        for(j = 0; j<4; j++)
            visited[i][j] = 0;
    
    Elem q[16];
    int counter = 1;
    int currctr = 0;
    q[0].act = cur;
    q[0].prev = -1;
    
    while(checker)
    {
        cur = q[currctr].act;
        
        int x = cur.x;
        int y = cur.y;
        visited[x][y] = 1;
        printf("Managing %d %d currctr %d counter %d\n", x, y, currctr, counter);
        
        if(available((XY){x+1, y}, (XY){x,y}, 4))
        {
            q[counter++] = (Elem){(XY){x+1, y}, currctr};
            visited[x+1][y] = 2;
        }
        if(available((XY){x-1, y}, (XY){x,y}, 3))
        {
            q[counter++] = (Elem){(XY){x-1, y}, currctr};
            visited[x-1][y] = 2;
        }
        if(available((XY){x, y+1}, (XY){x,y}, 1))
        {
            q[counter++] = (Elem){(XY){x, y+1}, currctr};
            visited[x][y+1] = 2;
        }
        if(available((XY){x, y-1}, (XY){x,y}, 2))
        {
            q[counter++] = (Elem){(XY){x, y-1}, currctr};
            visited[x][y-1] = 2;
        }
        currctr++;
    }
    
    for(i = 0; i<16; i++)
        printf("q[%d] = (Elem){(XY){%d, %d}, %d};\n", i, q[i].act.x, q[i].act.y, q[i].prev);
    
    counter--;
    while(!(cur.x == 0 && cur.y == 0))
    {
        cur = q[counter].act;
        printf("counter %d\n", counter);
        pushFront(&oP, cur);
        counter = q[counter].prev;
    }
    pushFront(&oP, (XY){0,0});
    
    printQueue(oP);
    return oP;
}


void goWithCutting(Queue** a)
{
    goSubOptimal(a);
}

void goOptimal()
{
    findFastest();
}



