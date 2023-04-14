/*
Concurrent test client
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "../lib/claves.h"


#define MAX_VALUE1 256
#define NUM_THREADS 4


// mutex vars
pthread_mutex_t mutex_id;
pthread_cond_t c_i;  // variable condicional de bloqueo
bool copiado = false;  // variable condicional de control


void workload(int* number) {
    /*
    Workload for the concurrent testing
    */

    init();

    // copy number
    pthread_mutex_lock(&mutex_id);
	int id = *number;
	copiado = true;  // update conditional variable
	pthread_cond_signal(&c_i);  // signal one sleeping thread
	pthread_mutex_unlock(&mutex_id);

    // do stuff
    char str[] = "caca";
    
    // set own number
    if (set_value(id, str, id, (double) id) != 0) {
        perror("-Failed to set value\n");
    }
    // set new key
    if (set_value(NUM_THREADS * id, str, id, (double) id) != 0) {
        perror("-Failed to set value\n");
    }

    exist(id);

    pthread_exit(NULL);
}


int main() {

    pthread_t thid[NUM_THREADS];

    // launch threads
    for (int i = 0; i < NUM_THREADS; i++) {
        // create thread
        if (pthread_create(&thid[i], NULL, (void*) workload, (void*) &i) == 0) {  // wait to copy i
            // mutex logic
            pthread_mutex_lock(&mutex_id);

            while (!copiado) {  // wait for thread to copy i
                pthread_cond_wait(&c_i, &mutex_id);
            }

            copiado = false;  // reset variable
            pthread_mutex_unlock(&mutex_id);

        } else {
            perror("Error al crear el thread\n");
            return -1;
        }
    }
    
    // wait for threads to end
	for (int j = 0; j < NUM_THREADS; j++) {
		pthread_join(thid[j], NULL);
	}

    // cleanup
    pthread_cond_destroy(&c_i);
	pthread_mutex_destroy(&mutex_id);

    printf("\n---Concurrent tests Successful---\n\n");
    
    return 0;
}