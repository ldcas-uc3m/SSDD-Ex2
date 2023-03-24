/*
Definition of concurrent linked list library
*/

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_


struct Node {
    int key;
    char* value1;
    int value2;
    double value3;
    struct Node *next;
};

typedef struct Node* List;


int initList(List* l);
int setKey(List* l, int key, char* value1, int value2, double value3);
int getKey(List l, int key, char* value1, int* value2, double* value3);
int existKey(List l, int key);
int modifyKey(List* l, int key, char* value1, int value2, double value3);
int copyKey(List* l, int key1, int key2);
int printList(List l);
int deleteKey(List* l, int key);
int destroyList(List* l);

#endif