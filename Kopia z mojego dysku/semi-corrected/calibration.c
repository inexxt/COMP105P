#include "defines.h"
#include "basicMovement.h"

int main()
{
    connect_to_robot();
    initialize_robot();
    int choice = 1;
    while(choice)
    {
        printf("Choose an action: \n");
        printf("1. Go X cm forward\n");
        printf("2. Turn by X degree\n");
        printf("Anything else. Quit\n");
        scanf("%d",&choice);
        float distance, angle;
        switch(choice)
        {
            case 1:
            printf("enter distance in cm you wish to move by: \n");
            scanf("%f",&distance);
            goStraight(distance);
            break;
            case 2:
            printf("enter the angle you wish to rotate by: \n");
            scanf("%f",&angle);
            turnByAngleDegree(angle);
            break;
            default:
            choice = 0;
            break;
        }
    }
    return 0;
}