#ifndef CYLIST_H_INCLUDED
#define CYLIST_H_INCLUDED

struct Cycle {
    int index;
    struct Cycle *next;
};

typedef struct Cycle * cycleNode;

struct CycleList {
    cycleNode current;
    struct CycleList *next;
};

typedef struct CycleList* cyListNode;

cycleNode addcycleNode(cycleNode head, int value);
cyListNode addcyListNode(cyListNode head, cycleNode newCurrent);
#endif // CYLIST_H_INCLUDED
