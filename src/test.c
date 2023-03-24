#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "tests/test_linked_list.h"
#include "lib/claves.h"


#define SERVER_PATH "servidor"
#define MAX_VALUE1 256


int testFullSeq() {
    /*
    Tests both server and client sequentially
    */

    // thread stuff
    pthread_attr_t t_attr;
    pthread_t thid;

	pthread_attr_init(&t_attr);


    // start client
    init();

    int key = 0;
    char value1[] = "caca";
    int value2 = 69;
    double value3 = 2.4;  

    if (set_value(key, value1, value2, value3) != 0){
        printf("Error while inserting the value");
        return -1;
    }

    if (shutdown() != 0){
        return -1;
    }

    return 0;
}


int main() {
    // test linked list
    if (testListSeq() == -1) {
        perror("\n---Sequential List tests Failed---\n\n");
        return -1;
    }

    if (testListConc() == -1) {
        perror("\n---Concurrent List tests Failed---\n\n");
        return -1;
    }

    if (testFullSeq() == -1) {
        perror("\n---Full test Failed---\n\n");
        return -1;
    }

    return 0;
}