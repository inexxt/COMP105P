#include <stdlib.h>
#include "lists.h"
#include <stdio.h>
#include "static_functions.h"
#include "defines.h"
#include "picomms.h"

double computeAngleList(node_t * head) 
{
    node_t * current = head;
	double dl = 0;
	double dr = 0;
	double alpha = 0;
	
	current = current->next;
    while (current != NULL) 
	{
        dl = current->l;
		dr = current->r;
		
		alpha += computeAngle(dl, dr);
		printf("%.2f %.2f\n", alpha, computeAngle(dl, dr));
        current = current->next;
    }
    return alpha;
}

double computeWayList(node_t * head, int which) 
{
    node_t * current = head;
	double dl = 0;
	double dr = 0;
	double alpha = 0;
	double total_alpha = 0;
	double wV = 0;
	double wH = 0;
	current = current->next;
    while (current != NULL) 
	{
        dl = current->l;
		dr = current->r;
		
		alpha = computeAngle(dl, dr);
		total_alpha += alpha;
		
		wV = wV + computeWay(dl, dr, alpha, VERTICAL)*cos(total_alpha);
		wH = wH + computeWay(dl, dr, alpha, HORIZONTAL)*sin(total_alpha);
// 		printf("alpha %.2f \n", alpha);
// 		printf("dl %.2f dr %.2f \n", dl, dr);
// 		printf("cwV %.2f cwH %.2f \n", computeWay(dl, dr, alpha, VERTICAL), computeWay(dl, dr, alpha, HORIZONTAL));
		
		set_point((int)(wH), (int)(wV));
		
        current = current->next;
    }
    if(which == VERTICAL) return wV;
	if(which == HORIZONTAL) return wH;
}

void print_list(node_t * head) 
{
    node_t * current = head;

    while (current != NULL) 
	{
        printf("%.2f %.2f\n", current->l, current->r);
        current = current->next;
    }
}

double lastL(node_t * head)
{
	node_t * current = head;
    while (current->next != NULL) 
	{
        current = current->next;
    }
    return current->l;
}

double lastR(node_t * head)
{
	node_t * current = head;
    while (current->next != NULL) 
	{
        current = current->next;
    }
    return current->r;
}

void push(node_t * head, double l, double r) 
{
    node_t * current = head;
    while (current->next != NULL) 
	{
        current = current->next;
    }

    current->next = malloc(sizeof(node_t));
    current->next->l = l;
	current->next->r = r;
    current->next->next = NULL;
}