#include <stdlib.h>
#include "defines.h"

typedef struct node {
    double l;
	double r;
    struct node * next;
} node_t;

double computeAngleList(node_t * head);
double computeWayList(node_t * head, int which);
void print_list(node_t * head);
double lastL(node_t * head);
double lastR(node_t * head);
void push(node_t * head, double l, double r);