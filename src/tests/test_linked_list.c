/*
Test program for the linked list library
*/

#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/linked_list.h"
#include "../lib/comm.h"
#include "test_linked_list.h"


#define NUM_THREADS 4


// mutex vars
pthread_mutex_t mutex_i;
pthread_cond_t c_i;  // variable condicional de bloqueo
bool copiado = false;  // variable condicional de control


// list
List list;


int testListSeq() {
    /*
    Test the linked list library sequentially
    */

    printf("\n---Start Sequential List tests---\n\n");


    // create list
    if (initList(&list) == 0) {
        printf("List created @ %p.\n", &list);
    } else return -1;

    // setKey()
    char str[] = "pene";
    if (setKey(&list, 420, str, 69, 420.69) == 0) {
        printf("Inserted (420, \"pene\", 69, 420.69)\n");
    } else return -1;

    if (setKey(&list, 421, str, 69, 420.69) == 0) {
        printf("Inserted (421, \"pene\", 69, 420.69)\n");
    } else return -1;

    // try duplicate
    if (setKey(&list, 420, str, 69, 420.69) == -1) {
        printf("Failed to insert (420, \"pene\", 69, 420.69)\n");
    } else return -1;

    printf("\nList: ");
    printList(list);
    printf("\n");


    // getKey()
    char value1[MAX_VALUE1];
    int value2;
    double value3;
    getKey(list, 420, value1, &value2, &value3);
    printf("Retrieved (420, %s, %i, %f).\n", value1, value2, value3);

    if (getKey(list, 69, value1, &value2, &value3) == -1) {
        printf("Failed to retrieve key 69.\n");
    } else return -1;

    // existKey()
    if (existKey(list, 420) == 1) {
        printf("Found 420.\n");
    } else return -1;
    
    if (existKey(list, 69) == 0) {
        printf("Failed to find key 69.\n");
    } else return -1;

    // modifyKey()
    strcpy(str, "caca");
    if (modifyKey(&list, 420, str, 70, 421.69) == 0) {
        printf("Modified key 420 to (420, \"caca\", 70, 421.69).\n");
    } else return -1;

    if (modifyKey(&list, 69, str, 70, 421.69) == -1) {
        printf("Failed to modifyKey key 69.\n");
    } else return -1;

    printf("\nList: ");
    printList(list);
    printf("\n");

    // copyKey()
    if (copyKey(&list, 420, 404) == 0) {
        printf("Copied key 420 to new key 404.\n");
    } else return -1;

    if (copyKey(&list, 420, 420) == -1) {
        printf("Failed to copy key 420 to new key 420.\n");
    } else return -1;
    
    if (copyKey(&list, 405, 406) == -1) {
        printf("Failed to copy key 405 to new key 406.\n");
    } else return -1;

    printf("\nList: ");
    printList(list);
    printf("\n");

    
    // deleteKey()
    if (deleteKey(&list, 404) == 0) {
        printf("Deleted node 404.\n");
    } else return -1;
    
    if (deleteKey(&list, 404) == -1) {
        printf("Failed to delete node 404.\n");
    } else return -1;

    printf("\nList: ");
    printList(list);
    printf("\n");

    if (destroyList(&list) == 0) {
        printf("Deleted list.\n");
    }

    // printList is already tested

    printf("\n---Sequential List tests Successful---\n\n");
    return 0;
}


void workload(int* number) {
    /*
    Workload for the concurrent testing
    */

    // copy number
    pthread_mutex_lock(&mutex_i);
	int i = *number;
	copiado = true;  // update conditional variable
	pthread_cond_signal(&c_i);  // signal one sleeping thread
	pthread_mutex_unlock(&mutex_i);

    // do stuff
    char str[] = "caca";
    setKey(&list, i, str, 420, 420.69);
    existKey(list, i + 1);
    setKey(&list, i + 1, str, 420, 420.69);

    modifyKey(&list, i, str, 421, 420.69);

    copyKey(&list, i, i * NUM_THREADS);

    deleteKey(&list, i);

    pthread_exit(NULL);
}


int testListConc() {
    /*
    Test the linked list library concurrently
    */

    pthread_t thid[NUM_THREADS];

    printf("\n---Start Concurrent List tests---\n\n");

    // init list
    initList(&list);


    // launch threads
    for (int i = 0; i < NUM_THREADS; i++) {
        // create thread
        if (pthread_create(&thid[i], NULL, (void*) workload, (void*) &i) == 0) {  // wait to copy i
            // mutex logic
            pthread_mutex_lock(&mutex_i);

            while (!copiado) {  // wait for thread to copy i
                pthread_cond_wait(&c_i, &mutex_i);
            }

            copiado = false;  // reset variable
            pthread_mutex_unlock(&mutex_i);

        } else {
            perror("Error al crear el thread\n");
            return -1;
        }
    }

    // wait for threads to end
	for (int j = 0; j < NUM_THREADS; j++) {
		pthread_join(thid[j], NULL);
	}

    printList(list);

    // cleanup
    destroyList(&list);

    pthread_cond_destroy(&c_i);
	pthread_mutex_destroy(&mutex_i);

    printf("\n---Concurrent List tests Successful---\n\n");
    return 0;
}


int main() {
    
    if (testListSeq() == -1) {
        perror("\n---Sequential List tests Failed---\n\n");
        return -1;
    }

    if (testListConc() == -1) {
        perror("\n---Concurrent List tests Failed---\n\n");
        return -1;
    }

    return 0;
}