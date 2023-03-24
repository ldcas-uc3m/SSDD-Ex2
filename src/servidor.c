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


// sync petitions
pthread_mutex_t mutex_pet;  // mutex for petition
pthread_cond_t c_pet;  // variable condicional de bloqueo
bool copiado = false;  // variable condicional de control

// sync printfs
pthread_mutex_t mutex_stdout;  // mutex for stdout

// shutdown
bool shutdown = false;
pthread_mutex_t mutex_shutdown;



void tratar_peticion(struct Peticion* p) {
    struct Peticion pet;
    
    // copy petition
	pthread_mutex_lock(&mutex_pet);

    pet.opcode = p->opcode;
    pet.cola_client = p->cola_client;
    pet.alt_key = p->alt_key;
    strcpy(pet.value.value1, p->value.value1);
    pet.value.value2 = p->value.value2;
    pet.value.value3 = p->value.value3;

    copiado = true;  // update conditional variable
	pthread_cond_signal(&c_pet);  // awake main
	pthread_mutex_unlock(&mutex_pet);


    // treat petition
    struct Respuesta res;

    pthread_mutex_lock(&mutex_stdout);
    printf("%i: Received {opcode: %i, key: %i, value1: %s, value2: %i, value3: %f} from %s\n", getpid(), pet.opcode, pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3, pet.cola_client);
    pthread_mutex_unlock(&mutex_stdout);

    switch (pet.opcode) {
        case 0:  // init
            res.result = init();
            break;

        case 1:  // set
            res.result = set(pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);
            break;

        case 2:  // get
            res.result = get(pet.value.clave, res.value.value1, &(res.value.value2), &(res.value.value3));
            break;
        
        case 3:  // modify
            res.result = modify(pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);
            break;
        
        case 4:  // exist
            res.result = exist(pet.value.clave);
            break;

        case 5:  // copyKey
            res.result = copy_key(pet.value.clave, pet.alt_key);
            break;

        case 6:  // shutdown
            pthread_mutex_lock(&mutex_shutdown);
            shutdown = true;
            pthread_mutex_unlock(&mutex_shutdown);
            break;

        default:
            res.result = -1;
            perror("Undefined operation code\n");

            break;
    }

    // answer
    mqd_t qc = mq_open(pet.cola_client, O_WRONLY);  // client queue
    if (qc == -1) {
        perror("No se puede abrir la cola del cliente");

        pthread_exit(NULL);
    }
    mq_send(qc, (const char*) &res, sizeof(res), 0);
    mq_close(qc);

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
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
	pthread_cond_init(&c_pet, NULL);
	pthread_mutex_init(&mutex_pet, NULL);
	pthread_mutex_init(&mutex_shutdown, NULL);
	pthread_mutex_init(&mutex_stdout, NULL);

	pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);


    /* MAIN LOOP */
    while (true) {
        // receive message
        mq_receive(qs, (char*) &msg, sizeof(struct Peticion), 0);

        // create thread
        if (pthread_create(&thid, &t_attr, (void*) tratar_peticion, (void*) &msg) == 0) {  // wait to copy petition
            // mutex logic
            pthread_mutex_lock(&mutex_pet);

            while (!copiado) {  // wait for thread to copy petition
                pthread_cond_wait(&c_pet, &mutex_pet);
            }

            copiado = false;  // reset variable
            pthread_mutex_unlock(&mutex_pet);

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

	pthread_cond_destroy(&c_pet);
	pthread_mutex_destroy(&mutex_pet);
	pthread_mutex_destroy(&mutex_shutdown);
	pthread_mutex_destroy(&mutex_stdout);

	exit(0);
}