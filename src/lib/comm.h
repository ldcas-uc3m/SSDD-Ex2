/*
Definición de las estructuras de Petición y Respuesta
*/

#ifndef _COMM_H_
#define _COMM_H_

#define MAX_NAME_COLA 80
#define MAX_VALUE1 256
#define MQUEUE_SIZE 10
#define SERVER_Q_NAME "/Super-cool-server-queue-thingy"


struct Tupla {
    /*
    Define la tupla donde se va a guardar la información
    */
   
    int clave;
    char value1[MAX_VALUE1];
    int value2;
    double value3;
};


struct Peticion {
    /*
    Petición del cliente al servidor

    OPCODE: 
        - 0: init
        - 1: set_value
        - 2: get_value
        - 3: modify_value
        - 4: exist
        - 5: copy_key
        - 6: shutdown
    NOMBRE: nombre de la cola del servidor
    COLA_CLIENT: nombre de la cola del cliente
    VALUE: tupla con los datos que se van a almacenar
    ALT_KEY: almacenará key2 para copy_key()
    */

    int opcode;
    char nombre[MAX_NAME_COLA];
    char* cola_client;
    struct Tupla value;
    int alt_key;
};

struct Respuesta {
    /*
    Respuesta del cliente al servidor
    
    RESULT: Resultado de la operación
    VALUE: tupla con los datos que se retornan
    */

    int result;
    struct Tupla value;
};


#endif