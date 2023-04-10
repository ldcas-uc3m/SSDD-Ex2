#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/server_impl.h"
#include "lib/comm.h"
#include "lib/lines.h"



// sync petitions
pthread_mutex_t mutex_sd;  // mutex for petition
pthread_cond_t c_sd;  // variable condicional de bloqueo
bool copiado = false;  // variable condicional de control

// sync printfs
pthread_mutex_t mutex_stdout;  // mutex for stdout

// shutdown
bool shutdown = false;
pthread_mutex_t mutex_shutdown;


void sigintHandler(int sig_num) {
    // signal handler for SIGINT

    pthread_mutex_lock(&mutex_shutdown);
    shutdown = true;
    pthread_mutex_unlock(&mutex_shutdown);

    printf("Shutting down...\n");
    fflush(stdout);
}


void *tratar_peticion(int* sd) {
    struct Peticion pet;
    char buff[MAX_LINE];

    // copy petition
	pthread_mutex_lock(&mutex_sd);

    int local_sd = *sd;  // copy sd

    copiado = true;  // update conditional variable
	pthread_cond_signal(&c_sd);  // awake main
	pthread_mutex_unlock(&mutex_sd);


    // TODO: handle petition

    pet.opcode = p->opcode;
    pet.cola_client = p->cola_client;
    pet.alt_key = p->alt_key;
    strcpy(pet.value.value1, p->value.value1);
    pet.value.value2 = p->value.value2;
    pet.value.value3 = p->value.value3;



    // treat petition
    struct Respuesta res;

    pthread_mutex_lock(&mutex_stdout);
    printf("%i: Received {opcode: %i, key: %i, value1: %s, value2: %i, value3: %f} from %s\n", getpid(), pet.opcode, pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3, pet.cola_client);
    pthread_mutex_unlock(&mutex_stdout);

    switch (pet.opcode) {
        case 0:  // init
            res.result = init();

            // answer
            sendMessage(local_sd, res.result, sizeof(res.result));
            break;

        case 1:  // set
            res.result = set(pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);

            // answer
            sendMessage(local_sd, res.result, sizeof(res.result));
            break;

        case 2:  // get
            res.result = get(pet.value.clave, res.value.value1, &(res.value.value2), &(res.value.value3));

            // answer
            sendMessage(local_sd, res.result, sizeof(res.result));
            break;
        
        case 3:  // modify
            res.result = modify(pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);

            // answer
            sendMessage(local_sd, res.result, sizeof(res.result));
            break;
        
        case 4:  // exist
            res.result = exist(pet.value.clave);

            // answer
            sendMessage(local_sd, res.result, sizeof(res.result));
            break;

        case 5:  // copyKey
            res.result = copy_key(pet.value.clave, pet.alt_key);

            // answer
            int result = htonl(res.result);
            sendMessage(local_sd, &result, sizeof(res.result));
            break;

        default:
            res.result = -1;
            perror("Undefined operation code\n");

            break;
    }

    
    close(local_sd);

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    // signal handler
    signal(SIGINT, sigintHandler);

    // TODO: replace queue

    // queue stuff
    mqd_t qs;  // server queue
    struct mq_attr q_attr;
    struct Peticion msg;  // message to receive

    q_attr.mq_maxmsg = MQUEUE_SIZE;
    q_attr.mq_msgsize = sizeof(struct Peticion);

    // thread stuff
    pthread_attr_t t_attr;
    pthread_t thid;


    // init queue
    qs = mq_open(SERVER_Q_NAME, O_CREAT|O_RDONLY, 0700, &q_attr);
    if (qs == -1) {
        perror("No se puede crear la cola del servidor\n");
        return -1;
    }


    // init mutex and cond
	pthread_cond_init(&c_sd, NULL);
	pthread_mutex_init(&mutex_sd, NULL);
	pthread_mutex_init(&mutex_shutdown, NULL);
	pthread_mutex_init(&mutex_stdout, NULL);

	pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);


    /* MAIN LOOP */
    while (true) {
        // TODO: replace queue

        // receive message
        mq_receive(qs, (char*) &msg, sizeof(struct Peticion), 0);

        // create thread
        if (pthread_create(&thid, &t_attr, (void*) tratar_peticion, (void*) &msg) == 0) {  // wait to copy petition
            // mutex logic
            pthread_mutex_lock(&mutex_sd);

            while (!copiado) {  // wait for thread to copy petition
                pthread_cond_wait(&c_sd, &mutex_sd);
            }

            copiado = false;  // reset variable
            pthread_mutex_unlock(&mutex_sd);

        } else {
            perror("Error al crear el thread\n");
            return -1;
        }

        // exit condition
        pthread_mutex_lock(&mutex_shutdown);
        if (shutdown) break;
        pthread_mutex_unlock(&mutex_shutdown);
    }


    // cleanup
    mq_close(qs);
    destroy();

	pthread_cond_destroy(&c_sd);
	pthread_mutex_destroy(&mutex_sd);
	pthread_mutex_destroy(&mutex_shutdown);
	pthread_mutex_destroy(&mutex_stdout);

	exit(0);
}