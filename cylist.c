#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct Cycle {
    int index;
    struct Cycle *next;
};

typedef struct Cycle * cycleNode;

cycleNode createcycleNode(){
    cycleNode temp;
    temp = (cycleNode) malloc(sizeof(struct Cycle));
    temp->next = NULL;
    return temp;
}

//inserisco in fondo
cycleNode addcycleNode(cycleNode head, int value){
    cycleNode temp, p;
    temp = createcycleNode();
    temp->index = value;
    if(head == NULL){
        head = temp;
    }
    else {
        p = head;
        while(p->next != NULL){
            p = p->next;
        }
        p->next = temp;
    }
    return head;
}

/*void free_CycleNode(cycleNode head) {
	cycleNode tmp;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		tmp->next = NULL;
		free(tmp);
		tmp = NULL;
	}
}*/


struct CycleList {
    cycleNode current;
    struct CycleList *next;
};

typedef struct CycleList* cyListNode;

cyListNode createCyListNode(){
    cyListNode temp;
    temp = (cyListNode) malloc(sizeof(struct CycleList));
    temp->next = NULL;
    return temp;
}

//inserisco in cima
cyListNode addcyListNode(cyListNode head, cycleNode newCurrent){
    cyListNode temp;
    temp = createCyListNode();
    temp->current = newCurrent;
    if(head == NULL){
        head = temp;
    }
    else {
        temp->next = head;
        head = temp;
    }
    return head;
}

/*void free_CycleList(cyListNode head) {
	cyListNode tmp;
    cycleNode current;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		current = tmp->current;
		tmp->next = NULL;
		free_CycleNode(current);
		free(tmp);
        current = NULL;
		tmp = NULL;
	}
}*/



