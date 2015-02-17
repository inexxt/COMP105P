#include <stdlib.h>
#include "lists.h"
#include <stdio.h>
// typedef struct node {
//     int l;
// 	int r;
//     struct node * next;
// } node_t;

void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%d %d\n", current->l, current->r);
        current = current->next;
    }
}

int lastL(node_t * head)
{
	node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current->l;
}


int lastR(node_t * head)
{
	node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current->r;
}


void push(node_t * head, int l, int r) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(node_t));
    current->next->l = l;
	current->next->r = r;
    current->next->next = NULL;
}