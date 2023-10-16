#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//struct of any list's node, generally nodes are considered in last-first order
typedef struct node {
    char *factor;
    struct node *next;
} node_t;

typedef struct nodeInt {
    int data;
    struct Node* next;
} node_int_t;


void free_list(node_t* head) {
	node_t* tmp;
	char *fact;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		fact = tmp->factor;
		//free(tmp->factor);
		tmp->factor = NULL;
		tmp->next = NULL;
		free(tmp);
		free(fact);
		fact = NULL;
		tmp = NULL;
	}
}

int count_for_print;
//recursive print of nodes in first-last order
void print_list_reverse(node_t *node) {
	if (node->next != NULL) {
		count_for_print++;
		print_list_reverse(node->next);
	} else {
		printf("[ ");
	}
	printf("\"%s\" ", node->factor);
	if (count_for_print == 0) {
		printf("]");
		count_for_print = 1;
	}
	count_for_print--;
}

void print_list(node_t *node) {
	printf("[ ");
	while (node != NULL) {
		printf("\"%s\" ", node->factor);
		node = node->next;
	}
	printf("]");
}

char *substring(const char* word, int x, int y) {
	int k = 0, i;
	char *sub = malloc((y-x + 1));

	for (i = x; i < y; i++) {
		sub[k++] = word[i];
	}
	sub[k] = '\0';

	return sub;
}

node_t* addFactorNode(node_t* head, char* factor){
    node_t* temp;
    temp = malloc(sizeof(node_t));
    temp->next = NULL;
    temp->factor = factor;
    if(head == NULL){
        head = temp;
    }
    else {
        temp->next = head;
        head = temp;
    }
    return head;
}

node_t* insertBottom(node_t *head, char* factor) {
  node_t* current_node = head;
  node_t* new_node;
     while ( current_node != NULL && current_node->next != NULL) {
       current_node = current_node->next;
      }

  new_node = malloc(sizeof(node_t));
  new_node->factor = factor;
  new_node->next= NULL;
  if (current_node != NULL)
    current_node->next = new_node;
  else
     head = new_node;
return head;
}


