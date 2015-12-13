#include <stdlib.h>
#include "lists.h"
#include <stdio.h>
#include "static_functions.h"
#include "defines.h"

// typedef struct node {
//     int l;
// 	int r;
//     struct node * next;
// } node_t;

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
	double wV = 0;
	double wH = 0;
	
    while (current != NULL) 
	{
		
// 		printf("1 wV %.2f wH %.2f \n", wV, wH);
        dl = current->l;
		dr = current->r;
		
		printf("%.2f %.2f \n", dl, dr);
		alpha = computeAngle(dl, dr);
		
// 		if(dl != 0 && dr != 0)
// 		{
// 			printf("1 CWV %lf CWH %lf \n", computeWay(dl, dr, alpha, VERTICAL)+wV, computeWay(dl, dr, alpha, HORIZONTAL)+wH);
// 			printf("1 wV %lf wH %lf \n", wV, wH);
// 		
			wV = wV + computeWay(dl, dr, alpha, VERTICAL);
			wH = wH + computeWay(dl, dr, alpha, HORIZONTAL);
			printf("2 alpha %lf \n", alpha);
			printf("2 CWV %lf CWH %lf \n", (computeWay(dl, dr, alpha, VERTICAL)), computeWay(dl, dr, alpha, HORIZONTAL));
			printf("2 wV %lf wH %lf \n", wV, wH);
// 		}
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
        printf("%d %d\n", current->l, current->r);
        current = current->next;
    }
}

int lastL(node_t * head)
{
	node_t * current = head;
    while (current->next != NULL) 
	{
        current = current->next;
    }
    return current->l;
}

int lastR(node_t * head)
{
	node_t * current = head;
    while (current->next != NULL) 
	{
        current = current->next;
    }
    return current->r;
}

void push(node_t * head, int l, int r) 
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