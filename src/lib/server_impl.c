/*
Server implementation for tuples
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "server_impl.h"
#include "linked_list.h"


List list = NULL;


int init() {
    /*
    Initializes the list. If it alredy exists, it deletes it.
    */
    
    if (list != NULL) {  // reset list
        destroy(&list);
    }

    initList(&list);
    
    return 0;
}


int set(int key, char* value1, int value2, double value3) {
    /*
    Adds tuple to the list
    */

    return setKey(&list, key, value1, value2, value3);   
}


int get(int key, char* value1, int* value2, double* value3) {
    /*
    Gets specified tuple
    */

    return getKey(list, key, value1, value2, value3);   
}


int exist(int key) {
    /*
    Check if key exists
    */
    
    return existKey(list, key);
}


int modify(int key, char* value1, int value2, double value3) {
    /*
    Modifies the key
    */
    
    return modifyKey(&list, key, value1, value2, value3);
}


int copy_key(int key1, int key2) {
    /*
    Copies the key1 to key2
    */

    return copyKey(&list, key1, key2);
}


int delete_key(int key) {
    /*
    Deletes the key
    */

    return deleteKey(&list, key);
}


void destroy() {
    /*
    Destroys the list and the mutexes.
    */

    destroyList(&list);

    return;
}
