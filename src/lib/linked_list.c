/*
Implementación de una linked list que guarda tuplas (int key, char value1[], int value2, double value3)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "linked_list.h"
#include "comm.h"


pthread_mutex_t mutex_list;
pthread_mutex_t mutex_stderr;

/*
Todas estas funciones devuelve 0 si se ejecutan con éxito y -1 en caso de algún error.
*/



int initList(List* l) {
    /*
    Inicializa una lista como lista vacía. 
    Una lista vacía es una lista que apunta a NULL.
    */
    
    // init mutex
    pthread_mutex_init(&mutex_list, NULL);
    pthread_mutex_init(&mutex_stderr, NULL);

    // start list
    *l = NULL;

    return 0;
}


int setKey(List* l, int key, char* value1, int value2, double value3) {
    /*
    Inserta una nueva tupla en la lista l.
    La inserción se hace al principio de la lista.
    Comprueba si hay keys repetidas.
    */

    // traverse the list
    List aux = *l;  // head

    pthread_mutex_lock(&mutex_list);
    while (aux != NULL) {
        if (aux->key == key) {  // key is already inserted

            pthread_mutex_lock(&mutex_stderr);
            perror("Key already inserted\n");
            pthread_mutex_unlock(&mutex_stderr);

            pthread_mutex_unlock(&mutex_list);
            return -1;
        }
        else {  // next element
            aux = aux->next;
        }
    }
    pthread_mutex_unlock(&mutex_list);
    // aux is now tail
    
    // insert new element

    // new node
    struct Node* ptr;
    ptr = (struct Node*) malloc(sizeof(struct Node));
    
    if (ptr == NULL) {
        pthread_mutex_lock(&mutex_stderr);
        perror("malloc() fail\n");
        pthread_mutex_unlock(&mutex_stderr);

        return -1;
    }

    // allocate memory for value1
    ptr->value1 = (char*) malloc(MAX_VALUE1);  // new string
    if (ptr->value1 == NULL) {  // failed allocation
        free(ptr);

        pthread_mutex_lock(&mutex_stderr);
        perror("malloc() fail\n");
        pthread_mutex_unlock(&mutex_stderr);

        return -1;
    }

    // form node
    ptr->key = key;
    strcpy(ptr->value1, value1);
    ptr->value2 = value2;
    ptr->value3 = value3;
    ptr->next = NULL;


    // link to the list
    if (*l == NULL) {  // emtpy list, insert in head
        pthread_mutex_lock(&mutex_list);
        *l = ptr;
        pthread_mutex_unlock(&mutex_list);
    }
    else {  // insert in head
        pthread_mutex_lock(&mutex_list);
        ptr->next = *l;
        *l = ptr;
        pthread_mutex_unlock(&mutex_list);
    }
    return 0;
}


int getKey(List l, int key, char* value1, int* value2, double* value3) {
    /*
    Obtiene el valor asociado a una clave key en la lista l, y lo guarda en el argumento value.
    */

    List aux = l;  // head

    // traverse the list
    pthread_mutex_lock(&mutex_list);
    while (aux != NULL) {
        if (aux->key == key) {  // found
            strcpy(value1, aux->value1);
            *value2 = aux->value2;
            *value3 = aux->value3;

            pthread_mutex_unlock(&mutex_list);
            return 0;  
        } else {  // next element
            aux = aux->next;
        }
    }
    pthread_mutex_unlock(&mutex_list);

    pthread_mutex_lock(&mutex_stderr);
    perror("Element not found\n");
    pthread_mutex_unlock(&mutex_stderr);

    return -1;  // not found
}


int printList(List l) {
    /*
    Recorre todos los elementos de la lista l e imprime por cada uno su clave y valor.
    */

    List aux = l;

    printf("[");

    pthread_mutex_lock(&mutex_list);
    while (aux != NULL) {
        printf("(%i, %s, %i, %f)", aux->key, aux->value1, aux->value2, aux->value3);
        aux = aux->next;
        if (aux != NULL) {
            printf(", ");
        }
    }
    pthread_mutex_unlock(&mutex_list);

    printf("]\n");

    return 0;
}


int deleteKey(List* l, int key) {
    /*
    Elimina un par de la lista l, identificado por su clave key.
    */

    List aux, back;

    if (*l == NULL)  // lista vacia
        return -1;
    
    // primer elemento de la lista
    pthread_mutex_lock(&mutex_list);
    if (key == (*l)->key) {
        aux = *l;
        *l = (*l)->next;
        free(aux);

        pthread_mutex_unlock(&mutex_list);
        return 0;
    }
    pthread_mutex_unlock(&mutex_list);

    aux = *l;
    back = *l;

    // resto de elementos
    while (aux != NULL) {
        if (aux->key == key) {  // found
            back->next = aux->next;
            free(aux);

            return 0;  
        }
        else {
            back = aux;
            aux = aux->next;
        }
    }

    pthread_mutex_lock(&mutex_stderr);
    perror("Element not found\n");
    pthread_mutex_unlock(&mutex_stderr);

    return -1;  // not found
}


int existKey(List l, int key) {
    /*
    Checks if the key key exists in the list.
    Returns 1 if found, 0 if not found.
    */

    // traverse the list
    List aux = l;  // head

    pthread_mutex_lock(&mutex_list);
    while (aux != NULL) {
        if (aux->key == key) {  // found
            pthread_mutex_unlock(&mutex_list);
            return true;
        }
        else {  // next element
            aux = aux->next;
        }
    }
    pthread_mutex_unlock(&mutex_list);
    return false;
}


int modifyKey(List* l, int key, char* value1, int value2, double value3) {
    /*
    Modifies the element key
    */
    
    // traverse the list
    List aux = *l;  // head

    pthread_mutex_lock(&mutex_list);
    while (aux != NULL) {
        if (aux->key == key) {  // found
            strcpy(aux->value1, value1);
            aux->value2 = value2;
            aux->value3 = value3;

            pthread_mutex_unlock(&mutex_list);
            return 0;
        }
        else {  // next element
            aux = aux->next;
        }
    }
    pthread_mutex_unlock(&mutex_list);

    pthread_mutex_lock(&mutex_stderr);
    perror("Element not found\n");
    pthread_mutex_unlock(&mutex_stderr);

    return -1;  // not found
}


int copyKey(List* l, int key1, int key2) {
    /*
    Copies the Node with key1 to a new node with key2.
    Checks if key2 already exists 
    */

    if (key1 == key2) {
        pthread_mutex_lock(&mutex_stderr);
        perror("Key already inserted\n");
        pthread_mutex_unlock(&mutex_stderr);

        return -1;
    }

    // traverse the list
    List aux = *l;  // head
    List tmp = NULL;
    
    pthread_mutex_lock(&mutex_list);
    while (aux != NULL) {
        if (aux->key == key1) {  // found key1
            tmp = aux;
            break;
        }
        else if (aux->key == key2) {  // key2 is already inserted

            pthread_mutex_lock(&mutex_stderr);
            perror("Key2 already inserted\n");
            pthread_mutex_unlock(&mutex_stderr);

            pthread_mutex_unlock(&mutex_list);
            return -1;
        }
        else {  // next element
            aux = aux->next;
        }
    }
    pthread_mutex_unlock(&mutex_list);
    // aux is now tail
    if (tmp == NULL) { // key not found
        pthread_mutex_lock(&mutex_stderr);
        perror("Key1 not found\n");
        pthread_mutex_unlock(&mutex_stderr);

        return -1;
    } 

    // insert new key

    // new node
    struct Node* ptr;
    ptr = (struct Node*) malloc(sizeof(struct Node));
    
    if (ptr == NULL) {
        pthread_mutex_lock(&mutex_stderr);
        perror("malloc() fail\n");
        pthread_mutex_unlock(&mutex_stderr);

        return -1;
    }

    // allocate memory for value1
    ptr->value1 = (char*) malloc(MAX_VALUE1);  // new string
    if (ptr->value1 == NULL) {  // failed allocation
        free(ptr);

        pthread_mutex_lock(&mutex_stderr);
        perror("malloc() fail\n");
        pthread_mutex_unlock(&mutex_stderr);

        return -1;
    }

    // form node
    ptr->key = key2;
    strcpy(ptr->value1, tmp->value1);
    ptr->value2 = tmp->value2;
    ptr->value3 = tmp->value3;
    ptr->next = NULL;


    // link to the list (insert in head)
    pthread_mutex_lock(&mutex_list);
    ptr->next = *l;
    *l = ptr;
    pthread_mutex_unlock(&mutex_list);

    return 0;
}


int destroyList(List* l) {
    /*
    Elimina todos los elementos de la lista l.
    */

    List aux;

    pthread_mutex_lock(&mutex_list);

    while (*l != NULL) {
        aux = *l;
        *l = aux->next;
        free(aux->value1);
        free(aux);
    }

    pthread_mutex_unlock(&mutex_list);

    pthread_mutex_destroy(&mutex_list);
    pthread_mutex_destroy(&mutex_stderr);

    return 0;
}
