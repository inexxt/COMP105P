#include <stdlib.h>
#include "defines.h"

typedef struct node {
    int l;
	int r;
    struct node * next;
} node_t;

double computeAngleList(node_t * head);
double computeWayList(node_t * head, int which);
void print_list(node_t * head);
int lastL(node_t * head);
int lastR(node_t * head);
void push(node_t * head, int l, int r);