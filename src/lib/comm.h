/*
Definición de las estructuras de Petición y Respuesta
*/

#ifndef _COMM_H_
#define _COMM_H_

#include <endian.h>
#include <stdint.h>

#define MAX_VALUE1 256
#define SERVER_PUERTO 4500
#define SERVER_DIR 5


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
    VALUE: tupla con los datos que se van a almacenar
    ALT_KEY: almacenará key2 para copy_key()
    */

    int opcode;
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


void double_to_network(double *data) {
    uint64_t tmp;
    memcpy(&tmp, data, sizeof(double));
    tmp = htole64(tmp);
    memcpy(data, &tmp, sizeof(uint64_t));
}

void double_to_host(double *data) {
    uint64_t tmp;
    memcpy(&tmp, data, sizeof(double));
    tmp = le64toh(tmp);
    memcpy(data, &tmp, sizeof(uint64_t));
}

#endif