#ifndef LISTS_H
#define LISTS_H
#include <stdlib.h>
#include "defines.h"

typedef struct node //LEFT - X, RIGHT - Y
{
    double l;
	double r;
    struct node * next;
} node_t; 

extern double wH, wV;
extern node_t * head_M;
extern node_t * head_XY;

node_t * initialize_list(double l, double r);
double computeAngleList();
void computeWayList();
void print_list(node_t * head);
double lastL(node_t * head);
double lastR(node_t * head);
void push(node_t * head, double l, double r);
node_t * push_front(node_t * head, double l, double r);	
node_t * list_get(int index);
int find_closest(double V, double H);

#endif