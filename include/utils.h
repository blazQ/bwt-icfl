#ifndef UTILS_H_
#define UTILS_H_


typedef struct node {
    char *factor;
    struct node *next;
} node_t;

void free_list(node_t* head);
void print_list_reverse(node_t *node);
void print_list(node_t *node);
char * substring(const char* word, int x, int y);
node_t* addFactorNode(node_t* head, char* factor);
node_t* insertBottom(node_t *head, char* factor);
#endif /* UTILS_H_ */
