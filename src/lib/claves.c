/*
Implementación de las operaciones del cliente
*/

#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include "claves.h"
#include "comm.h"

#define INIT 0
#define SET_VALUE 1
#define GET_VALUE 2
#define MODIFY_VALUE 3
#define EXIST 4
#define COPY_KEY 5

#define NUM_MENSAJES 10

#define DOMINIO AF_INET

#define TIPO SOCK_STREAM

#define PROTOCOLO IPPROTO_TCP

int create_socket(int *socket_desc) {
    // Read env variables
    char *IP_SERVER_STR = getenv("IP_TUPLAS");
    char *PORT_SERVER_STR = getenv("PORT_TUPLAS");
    
    if (IP_SERVER_STR == NULL) {
        printf("Necesitas definir \"IP_TUPLAS\"");
        return -1;
    }
    if (PORT_SERVER_STR == NULL) {
        printf("Necesitas definir \"PORT_SERVER_STR\"");
        return -1;
    }

    short int PORT_SERVER = atoi(PORT_SERVER_STR);

    // Create socket
    int socket_fd;
	struct sockaddr_in server;

	socket_fd = socket(DOMINIO , TIPO , PROTOCOLO);
	
	if (socket_fd == -1)
	{
		printf("No se pudo crear el socket");
        return -1;
	}

    server.sin_addr.s_addr = inet_addr(IP_SERVER_STR);
	server.sin_family = AF_INET;
	server.sin_port = htons( PORT_SERVER );

	//Connect to remote server
	if (connect(socket_fd , (struct sockaddr *)&server , sizeof(server)) == -1)
	{
		printf("Error al connectar con el servidor");
		return -1;
	}

    *socket_desc = socket_fd;
    return 0;
}


int init(void) {

    int socket_desc;
    int status_create = create_socket(&socket_desc);

    if (status_create != 0) {
        return -1;
    }

    // Enviar peticion
    int opcode = htonl(INIT);
    send(socket_desc , &opcode , sizeof(opcode) , 0);
    
    
    // Recibir respuesta
    int res;
    read(socket_desc, &res , sizeof(res));
    res = ntohl(res);

    if (res == -1) {
        return -1;
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        return -1;
    }else{
        return 0;
    }
}


int set_value(int key, char* value1, int value2, double value3) {

    if (strlen(value1)>256){
        return -1;
    }

    char value1_[MAX_VALUE1];
    memcpy(value1_, value1, strlen(value1));
    

    int socket_desc;
    int status_create = create_socket(&socket_desc);

    if (status_create != 0) {
        return -1;
    }

    // Enviar peticion
    int opcode = htonl(SET_VALUE);
    send(socket_desc , &opcode , sizeof(opcode) , 0);

    int key_send = htonl(key);
    send(socket_desc , &key_send , sizeof(key_send) , 0);

    send(socket_desc , &value1_ , MAX_VALUE1, 0);

    int value2_send = htonl(value2);
    send(socket_desc , &value2_send , sizeof(value2_send) , 0);

    //send double
    double_to_network(&value3);
    send(socket_desc , &value3 , sizeof(value3) , 0);
    
    // Recibir respuesta
    int res;
    read(socket_desc, &res , sizeof(res));
    res = ntohl(res);

    if (res == -1) {
        return -1;
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        return -1;
    }

    return 0;
}


int get_value(int key, char* value1, int* value2, double* value3) {

    int socket_desc;
    int status_create = create_socket(&socket_desc);

    if (status_create != 0) {
        return -1;
    }

    // Enviar peticion
    int opcode = htonl(GET_VALUE);
    send(socket_desc , &opcode , sizeof(opcode) , 0);

    int key_send = htonl(key);
    send(socket_desc , &key_send , sizeof(key_send) , 0);
    
    
    // Recibir respuesta
    int res;
    read(socket_desc, &res , sizeof(res));
    res = ntohl(res);

    if (res == -1) {
        return -1;
    }


    read(socket_desc, value1 , MAX_VALUE1); 
    
    read(socket_desc, &value2 , sizeof(*value2));
    *value2 = ntohl(*value2);

    //receive double
    read(socket_desc, &value3 , sizeof(*value3));
    double_to_host(value3);

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        return -1;
    }

    return 0;
}


int modify_value(int key, char* value1, int value2, double value3) {


    if (strlen(value1)>256){
        return -1;
    }

    char value1_[MAX_VALUE1];
    memcpy(value1_, value1, strlen(value1));
    

    int socket_desc;
    int status_create = create_socket(&socket_desc);

    if (status_create != 0) {
        return -1;
    }

    // Enviar peticion
    int opcode = htonl(MODIFY_VALUE);
    send(socket_desc , &opcode , sizeof(opcode) , 0);

    int key_send = htonl(key);
    send(socket_desc , &key_send , sizeof(key_send) , 0);

    send(socket_desc , &value1_ , MAX_VALUE1, 0);

    int value2_send = htonl(value2);
    send(socket_desc , &value2_send , sizeof(value2_send) , 0);

    //send double
    double_to_network(&value3);
    send(socket_desc , &value3 , sizeof(value3) , 0);
    
    // Recibir respuesta
    int res;
    read(socket_desc, &res , sizeof(res));
    res = ntohl(res);

    if (res == -1) {
        return -1;
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        return -1;
    }

    return 0;
}


int exist(int key) {

    int socket_desc;
    int status_create = create_socket(&socket_desc);

    if (status_create != 0) {
        return -1;
    }

    // Enviar peticion
    int opcode = htonl(EXIST);
    send(socket_desc , &opcode , sizeof(opcode) , 0);

    int key_send = htonl(key);
    send(socket_desc , &key_send , sizeof(key_send) , 0);
    
    
    // Recibir respuesta
    int res;
    read(socket_desc, &res , sizeof(res));
    res = ntohl(res);

    if (res == -1) {
        return -1;
    }

    return res;
}


int copy_key(int key1, int key2) {
    
    int socket_desc;
    int status_create = create_socket(&socket_desc);

    if (status_create != 0) {
        return -1;
    }

    // Enviar peticion
    int opcode = htonl(COPY_KEY);
    send(socket_desc , &opcode , sizeof(opcode) , 0);

    int key1_send = htonl(key1);
    send(socket_desc , &key1_send , sizeof(key1_send) , 0);

    int key2_send = htonl(key2);
    send(socket_desc , &key2_send , sizeof(key2_send) , 0);
    
    
    // Recibir respuesta
    int res;
    read(socket_desc, &res , sizeof(res));
    res = ntohl(res);

    if (res == -1) {
        return -1;
    }

    return 0;
}
