#include <stdlib.h>
#include "lists.h"
#include <stdio.h>
#include "static_functions.h"
#include "defines.h"
#include "picomms.h"

double wH = 0, wV = 0;
node_t * head_M = NULL; 
node_t * head_XY = NULL;

node_t* initialize_list(double l, double r)
{
	node_t * head = NULL;
	head = malloc(sizeof(node_t));
	head->l = l;
	head->r = r;
	head->next = NULL;
	return head;
}

double computeAngleList()
{
    node_t * current = head_M;
	double dl = 0;
	double dr = 0;
	double alpha = 0;
	
	current = current->next;
	if(current == NULL) printf("XX\n");
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

void computeWayList() 
{
    node_t * currentM = head_M;
	head_XY = initialize_list(0, 0);
	
	node_t * currentXY = head_XY;
	double dl = 0;
	double dr = 0;
	double alpha = 0;
	double total_alpha = 0;
	double cw = 0;
	printf("XY\n");
	
	currentM = currentM->next;
	
    while (currentM != NULL) 
	{
        dl = currentM->l;
		dr = currentM->r;
		
		alpha = computeAngle(dl, dr);
		total_alpha += alpha;
		cw = computeWay(dl, dr, alpha);
		wV = wV + cw*cos(total_alpha);
		wH = wH + cw*sin(total_alpha);
// 		printf("alpha %.2f \n", alpha);
// 		printf("dl %.2f dr %.2f \n", dl, dr);
// 		printf("cwV %.2f cwH %.2f \n", computeWay(dl, dr, alpha, VERTICAL), computeWay(dl, dr, alpha, HORIZONTAL));
		
		set_point((int)(wH), (int)(wV));
		head_XY = push_front(head_XY, wV, wH);
		
        currentM = currentM->next;
    }
	return;
}

void print_list(node_t * head) 
{
	printf("Printing list: \n");
    node_t * current = head;

    while (current != NULL) 
	{
        printf("%.2f %.2f\n", current->l, current->r);
        current = current->next;
    }
}

int find_closest(double V, double H)
{
	int cls = 0;
	int indx = 0;
	node_t * currentXY = head_XY;
	double dist = distance(V, currentXY->l, H, currentXY->r);
	
    while (currentXY != NULL) 
	{
        if(distance(V, currentXY->l, H, currentXY->r) < dist)
		{
			cls = indx;
			dist = distance(V, currentXY->l, H, currentXY->r);
		}
		indx++;
        currentXY = currentXY->next;
    }
    return cls;
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
	printf("Entering push mode\n");
    current->next = malloc(sizeof(node_t));
    current->next->l = l;
	current->next->r = r;
    current->next->next = NULL;
}

node_t * push_front(node_t * head, double l, double r) 
{
	node_t * neww = malloc(sizeof(node_t));
    neww->l = l;
	neww->r = r;
    neww->next = head;
	return neww;
}

node_t * list_get(int index)
{
	int i = 0;
	node_t * current = head_XY;
    while (current->next->next != NULL && i < index) 
	{
        current = current->next;
		i++;
    }
    return current;
}
