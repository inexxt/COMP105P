#include "cuttingCorners.h"

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

void goSubOptimal(Queue** a)
{
    int i = 0;
    for(i=0; i<length; i++)
    {
        qP[i] = (XY){0, 0};
    }

    length = 0;
    XY help;
    while(!isEmpty(*a))
    {
        help = popFront(a);
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

            if(py != ay)
            {
                b1.x = ax;
                b1.y = by;

                b2.x = bx;
                b2.y = cy;
            }
            if(px != ax)
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