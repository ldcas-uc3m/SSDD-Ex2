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
#include "lines.h"

#define INIT 0
#define SET_VALUE 1
#define GET_VALUE 2
#define MODIFY_VALUE 3
#define EXIST 4
#define COPY_KEY 5
#define DELETE_KEY 6

#define NUM_MENSAJES 10
#define DOMINIO AF_INET
#define TIPO SOCK_STREAM
#define PROTOCOLO IPPROTO_TCP


int create_socket(int *socket_desc) {
    int socket_fd;
	struct sockaddr_in server_addr;

    // Read env variables
    char *IP_SERVER_STR = getenv("IP_TUPLAS");
    char *PORT_SERVER_STR = getenv("PORT_TUPLAS");

    if (strcmp(IP_SERVER_STR, "localhost") == 0) {
        IP_SERVER_STR = "127.0.0.1";
    }
    
    if (IP_SERVER_STR == NULL) {
        printf("Necesitas definir \"IP_TUPLAS\"\n");
        return -1;
    }
    if (PORT_SERVER_STR == NULL) {
        printf("Necesitas definir \"PORT_SERVER_STR\"\n");
        return -1;
    }

    short int PORT_SERVER = atoi(PORT_SERVER_STR);

    // printf("Connecting to %i:%s\n", PORT_SERVER, IP_SERVER_STR);

    // Create socket
	socket_fd = socket(DOMINIO , TIPO , PROTOCOLO);
	
	if (socket_fd == -1)
	{
		printf("No se pudo crear el socket");
        return -1;
	}

    // Obtain Server address 
    // struct hostent *hp;
    // bzero((char*) &server_addr, sizeof(server_addr));
    // hp = gethostbyname(IP_SERVER_STR);
    // if (hp == NULL) {
    //     perror("Error en gethostbyname");
    //     exit(1);
    // }

	// memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_addr.s_addr = inet_addr(IP_SERVER_STR);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_SERVER);

	//Connect to remote server
	if (connect(socket_fd , (struct sockaddr *)&server_addr , sizeof(server_addr)) == -1)
	{
		printf("Error al connectar con el servidor\n");
		return -1;
	}

    *socket_desc = socket_fd;
    return 0;
}


int init(void) {
    int socket_desc;
    int status_create = create_socket(&socket_desc);
    char buffer[MAX_LINE];

    if (status_create != 0) {
        return -1;
    }

    int res=0;
    // Enviar peticion
    sprintf(buffer, "%i", INIT);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in init\n");
        return -1;
    }
    
    
    // Recibir respuesta


    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine\n");
        close(socket_desc);
        return -1;
    }else if (res!=-1){
        res = atoi(buffer);
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        printf("Error al cerrar el socket\n");
        return -1;
    }else {
        return res;
    }
}


int set_value(int key, char* value1, int value2, double value3) {
    char buffer[MAX_LINE];

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

    int res=0;
    // Enviar peticion
    sprintf(buffer, "%i", SET_VALUE);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1)==-1){
        printf("Error senting the OPCODE\n");
        return -1;
    }

    sprintf(buffer, "%i", key);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error when sending the key\n");
        return -1;
    }

    if (sendMessage(socket_desc , value1_, strlen(value1) + 1) == -1){
        printf("Error when sending value 1\n");
        return -1;
    }
    
    sprintf(buffer, "%i", value2);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error when sending value 2\n");
        return -1;
    }


    sprintf(buffer, "%lf", value3);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error when sending value 3\n");
        return -1;
    }
    
    // Recibir respuesta
    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error reading the response\n");
        res = -1;
    }else if(res!=-1){
        res = atoi(buffer);
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        printf("Error al cerrar el socket\n");
        return -1;
    }

    return res;
}


int get_value(int key, char* value1, int* value2, double* value3) {
    char buffer[MAX_LINE];
    int socket_desc;
    int status_create = create_socket(&socket_desc);

    if (status_create != 0) {
        return -1;
    }

    int res=0;
    // Enviar peticion
    sprintf(buffer, "%i", GET_VALUE);
    if(sendMessage(socket_desc , buffer , strlen(buffer) + 1)==-1){
        printf("Error sending the OPCODE\n");
        return -1;
    }

    sprintf(buffer, "%i", key);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }
    
    // Recibir respuesta
    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine res\n");
        res=-1;
    }else if (res!=-1){
        res = atoi(buffer);
    }

    if (readLine(socket_desc, value1, MAX_LINE)==-1){
        printf("Error in readline value1\n");
        return -1;
    }
    
    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine value2\n");
        res=-1;
    }else if (res!=-1){
        *value2 = atoi(buffer);
    }
    

    //receive double
    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine value 3\n");
        int closing = close(socket_desc);
        if (closing ==-1){
            printf("Error al cerrar el socket\n"); 
        }
        res = -1;
    }else if (res!=-1){
        *value3 = atof(buffer);
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        printf("Error al cerrar el socket\n");
        return -1;
    }

    return res;
}


int modify_value(int key, char* value1, int value2, double value3) {
    char buffer[MAX_LINE];

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

    int res=0;

    // Enviar peticion
    sprintf(buffer, "%i", MODIFY_VALUE);
    if (sendMessage(socket_desc , buffer , strlen(buffer) + 1)==-1){
        printf("Error sending the OPCODE\n");
        return -1;
    }

    sprintf(buffer, "%i", key);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }

    if (sendMessage(socket_desc , value1_ , strlen(value1_) + 1)==-1){
        printf("Error in sendMessage\n");
    }
    
    sprintf(buffer, "%i", value2);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }

    sprintf(buffer, "%lf", value3);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }
    
    // Recibir respuesta
    
    
    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine\n");
        res=-1;
    }else if (res!=-1){
        res = atoi(buffer);
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        printf("Error al cerrar el socket\n");
        return -1;
    }

    return res;
}


int exist(int key) {
    int socket_desc;
    int status_create = create_socket(&socket_desc);
    char buffer[MAX_LINE];

    if (status_create != 0) {
        return -1;
    }

    int res=0;

    // Enviar peticion
    sprintf(buffer, "%i", EXIST);
    if (sendMessage(socket_desc , buffer , strlen(buffer) + 1)==-1){
        printf("Error in sendMessage\n");
        return -1;
    }

    sprintf(buffer, "%i", key);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }
    
    
    // Recibir respuesta
    
    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine\n");
        res=-1;
    }else if(res!=-1){
        res = atoi(buffer);
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        printf("Error al cerrar el socket\n");
        return -1;
    }

    return res;
}


int copy_key(int key1, int key2) {
    
    int socket_desc;
    int status_create = create_socket(&socket_desc);
    char buffer[MAX_LINE];

    if (status_create != 0) {
        return -1;
    }

    int res=0;
    // Enviar peticion
    sprintf(buffer, "%i", COPY_KEY);
    if (sendMessage(socket_desc , buffer , strlen(buffer) + 1)==-1){
        printf("Error in sendMessage\n");
        return -1;
    }

    sprintf(buffer, "%i", key1);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }
    
    sprintf(buffer, "%i", key2);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }
    
    
    // Recibir respuesta
    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine\n");
        res=-1;
    }else if(res!=-1){
        res = atoi(buffer);
    }

    

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        printf("Error al cerrar el socket\n");
        return -1;
    }

    return res;
}


int delete_key(int key){
    int socket_desc;
    int status_create = create_socket(&socket_desc);
    char buffer[MAX_LINE];

    if (status_create != 0) {
        return -1;
    }

    int res=0;
    // Enviar peticion
    sprintf(buffer, "%i", DELETE_KEY);
    if (sendMessage(socket_desc , buffer , strlen(buffer) + 1)==-1){
        printf("Error in sendMessage\n");
        return -1;
    }

    sprintf(buffer, "%i", key);
    if (sendMessage(socket_desc, buffer, strlen(buffer) + 1) == -1) {
        printf("Error in sendMessage\n");
        return -1;
    }
    
    // Recibir respuesta

    if (readLine(socket_desc, buffer, MAX_LINE) == -1) {
        printf("Error in readLine\n");
        res=-1;
    }else if(res!=-1){
        res = atoi(buffer);
    }

    // Close socket
    int closing = close(socket_desc);
    if (closing ==-1){
        printf("Error al cerrar el socket\n");
        return -1;
    }

    return res;
}