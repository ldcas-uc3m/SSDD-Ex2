#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "lib/server_impl.h"
#include "lib/comm.h"
#include "lib/lines.h"



// sync petitions
pthread_mutex_t mutex_sd;  // mutex for petition
pthread_cond_t c_sd;  // variable condicional de bloqueo
bool copiado = false;  // variable condicional de control

// sync printfs
pthread_mutex_t mutex_stdout;  // mutex for stdout

// shutdown_server
bool shutdown_server = false;
pthread_mutex_t mutex_shutdown_server;


void sigintHandler(int sig_num) {
    // signal handler for SIGINT

    pthread_mutex_lock(&mutex_shutdown_server);
    shutdown_server = true;
    pthread_mutex_unlock(&mutex_shutdown_server);

    printf("Shutting down...\n");
    fflush(stdout);
}


void *tratar_peticion(int* sd) {
    struct Peticion pet;

    // copy sd
	pthread_mutex_lock(&mutex_sd);

    int local_sd = *sd;  // copy sd

    copiado = true;  // update conditional variable
	pthread_cond_signal(&c_sd);  // awake main
	pthread_mutex_unlock(&mutex_sd);


    // read opcode
    read(local_sd, &(pet.opcode), sizeof(pet.opcode));
    pet.opcode = ntohl(pet.opcode);


    // treat petition
    struct Respuesta res;

    // pthread_mutex_lock(&mutex_stdout);
    // printf("%i: Received {opcode: %i, key: %i, value1: %s, value2: %i, value3: %f} from %s\n", getpid(), pet.opcode, pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3, pet.cola_client);
    // pthread_mutex_unlock(&mutex_stdout);

    switch (pet.opcode) {
        case 0:  // init

            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received init\n", local_sd);
            pthread_mutex_unlock(&mutex_stdout);

            // execute
            res.result = init();

            // answer
            res.result = htonl(res.result);
            write(local_sd, &(res.result), sizeof(res.result));
            break;

        case 1:  // set

            // get arguments
            read(local_sd, &(pet.value.clave), sizeof(pet.value.clave));
            pet.value.clave = ntohl(pet.value.clave);

            readLine(local_sd, &(pet.value.value1), MAX_VALUE1 + 1);

            read(local_sd, &(pet.value.value2), sizeof(pet.value.value2));
            pet.value.value2 = ntohl(pet.value.value2);

            read(local_sd, &(pet.value.value3), sizeof(pet.value.value3));
            double_to_host(&(pet.value.value3));

            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received {opcode %i (init), key: %i, value1: %s, value2: %i, value3: %f}\n", local_sd, pet.opcode, pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);
            pthread_mutex_unlock(&mutex_stdout);

            // execute
            res.result = set(pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);

            // answer
            res.result = htonl(res.result);
            write(local_sd, &(res.result), sizeof(res.result));
            
            break;

        case 2:  // get
            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received init\n", local_sd);
            pthread_mutex_unlock(&mutex_stdout);
            
            // get arguments
            read(local_sd, &(pet.value.clave), sizeof(pet.value.clave));
            pet.value.clave = ntohl(pet.value.clave);

            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received {opcode: %i (get), key: %i}\n", local_sd, pet.opcode, pet.value.clave);
            pthread_mutex_unlock(&mutex_stdout);

            // execute
            res.result = get(pet.value.clave, res.value.value1, &(res.value.value2), &(res.value.value3));

            // answer
            res.result = htonl(res.result);
            write(local_sd, &(res.result), sizeof(res.result));

            sendMessage(local_sd, res.value.value1, strlen(res.value.value1) + 1);

            write(local_sd, &(res.value.value2), sizeof(res.value.value2));

            double_to_network(&(res.value.value3));
            write(local_sd, &(res.value.value3), sizeof(res.value.value3));

            break;
        
        case 3:  // modify

            // get arguments
            read(local_sd, &(pet.value.clave), sizeof(pet.value.clave));
            pet.value.clave = ntohl(pet.value.clave);

            readLine(local_sd, &(pet.value.value1), MAX_VALUE1 + 1);

            read(local_sd, &(pet.value.value2), sizeof(pet.value.value2));
            pet.value.value2 = ntohl(pet.value.value2);

            read(local_sd, &(pet.value.value3), sizeof(pet.value.value3));
            double_to_host(&(pet.value.value3));

            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received {opcode %i (modify), key: %i, value1: %s, value2: %i, value3: %f}\n", local_sd, pet.opcode, pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);
            pthread_mutex_unlock(&mutex_stdout);

            // execute
            res.result = modify(pet.value.clave, pet.value.value1, pet.value.value2, pet.value.value3);

            // answer
            res.result = htonl(res.result);
            write(local_sd, &(res.result), sizeof(res.result));

            break;
        
        case 4:  // exist
            // get arguments
            read(local_sd, &(pet.value.clave), sizeof(pet.value.clave));
            pet.value.clave = ntohl(pet.value.clave);

            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received {opcode %i (exist), key: %i}\n", local_sd, pet.opcode, pet.value.clave);
            pthread_mutex_unlock(&mutex_stdout);

            // execute
            res.result = exist(pet.value.clave);

            // answer
            res.result = htonl(res.result);
            write(local_sd, &(res.result), sizeof(res.result));

            break;

        case 5:  // copyKey
            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received init\n", local_sd);
            pthread_mutex_unlock(&mutex_stdout);

            // get arguments
            read(local_sd, &(pet.value.clave), sizeof(pet.value.clave));
            pet.value.clave = ntohl(pet.value.clave);
            
            read(local_sd, &(pet.alt_key), sizeof(pet.alt_key));
            pet.alt_key = ntohl(pet.alt_key);

            pthread_mutex_lock(&mutex_stdout);
            printf("%i: Received {opcode %i (exist), key: %i, alt_key: %i}\n", local_sd, pet.opcode, pet.value.clave, pet.alt_key);
            pthread_mutex_unlock(&mutex_stdout);

            // execute
            res.result = copy_key(pet.value.clave, pet.alt_key);

            // answer
            res.result = htonl(res.result);
            write(local_sd, &(res.result), sizeof(res.result));

            break;

        default:
            res.result = -1;
            perror("Undefined operation code\n");

            // answer
            res.result = htonl(res.result);
            write(local_sd, &(res.result), sizeof(res.result));

            break;
    }

    
    close(local_sd);

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    int sd, newsd;
    socklen_t size;
    struct sockaddr_in server_addr, client_addr;


    // signal handler
    signal(SIGINT, sigintHandler);


    // read env variables
    char *PORT_SERVER_STR = getenv("PORT_TUPLAS");
    
    if (PORT_SERVER_STR == NULL) {
        printf("Necesitas definir \"PORT_SERVER_STR\"");
        return -1;
    }


    // thread stuff
    pthread_attr_t t_attr;
    pthread_t thid;


    // init mutex and cond
	pthread_cond_init(&c_sd, NULL);
	pthread_mutex_init(&mutex_sd, NULL);
	pthread_mutex_init(&mutex_shutdown_server, NULL);
	pthread_mutex_init(&mutex_stdout, NULL);

	pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);


    // configure socket
    if ((sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Error in socket");
        exit(1);
    }

    int val = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*) &val, sizeof(int)) < 0) {
        perror("Error in option");
        exit(1);
    }
    
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;


    // bind
    if (bind(sd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error in bind\n");
        return -1;
    }

    /* MAIN LOOP */
    listen(sd, SOMAXCONN);  // open socket

    while (true) {

        // accept petition
        newsd = accept(sd, (struct sockaddr*) &client_addr, &size);
        if (newsd < 0) {
            perror("Error in accept\n");
            return -1;
        }
        printf("Client %i connected", newsd);

        // create thread
        if (pthread_create(&thid, &t_attr, (void*) tratar_peticion, (void*) &newsd) == 0) {  // wait to copy petition
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
        pthread_mutex_lock(&mutex_shutdown_server);
        if (shutdown_server) break;
        pthread_mutex_unlock(&mutex_shutdown_server);
    }


    // cleanup
    close(sd);
    destroy();

	pthread_cond_destroy(&c_sd);
	pthread_mutex_destroy(&mutex_sd);
	pthread_mutex_destroy(&mutex_shutdown_server);
	pthread_mutex_destroy(&mutex_stdout);

	exit(0);
}

