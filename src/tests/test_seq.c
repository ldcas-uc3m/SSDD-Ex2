/*
Simple sequential test client
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../lib/claves.h"


#define MAX_VALUE1 256


int standard_run() {
    /*
    Tests all basic functions
    */

    // start list
    printf("Iniciando servicio\n");
    init();

    int key = 0;
    char value1[] = "Prueba";
    int value2 = 1;
    double value3 = 2.4;  

    // set
    printf("\nProbando set\n");
    if (set_value(key, value1, value2, value3) != 0) {
        perror("Error while inserting the value\n");
        return -1;
    }

    // check previous key exists
    printf("\nProbando exist\n");
    if (exist(key) != 1) {
        perror("Key doesn't exist\n");
        return -1;
    }

    // copy key
    int newKey = 1;
    printf("\nProbando copy_key\n");
    if (copy_key(key, newKey) != 0) {
        perror("Error while copying the key\n");
        return -1;
    }

    // modify value
    int newValue2 = 2;
    printf("\nProbando modify_value\n");

    if (modify_value(newKey, value1, newValue2, value3) != 0) {
        perror("Error while modifying the tuple\n");
        return -1;
    }

    // get
    char* value1Get = malloc(MAX_VALUE1 * sizeof(char));
    int value2Get;
    double value3Get;

    printf("\nProbando get_value\n");

    if (get_value(newKey, value1Get, &value2Get, &value3Get) == 0) {
        printf("The obtained values are %s , %d, %f\n", value1Get, value2Get, value3Get);
    }
    else {
        free(value1Get);
        perror("Error while getting the key\n");
        return -1;
    }
    
    // delete_key
    printf("\nProbando delete_key\n");
    if (delete_key(newKey) != 0) {
        perror("Error while deleting the key\n");
        return -1;
    }

    return 0;
}


int break_stuff() {
    /*
    Let's break some stuff!
    Just to see this works...
    */

    // see if it refreshes when init
    printf("\nProbando doble init\n");
    init();

    int key = 0;
    char value1[] = "Prueba";
    int value2 = 1;
    double value3 = 2.4;  

    // set value
    if (set_value(key, value1, value2, value3) != 0) {
        perror("Error while inserting the value\n");
        return -1;
    }

    init();

    if (exist(key) == 1) {
        perror("Init doesn't refresh\n");
        return -1;
    }

    return 0;
}


int main() {
    if (standard_run() == -1) {
        perror("\n---Sequential tests Failed---\n\n");
        return -1;
    }

    if (break_stuff() == -1) {
        perror("\n---Sequential tests Failed---\n\n");
        return -1;
    }

    printf("\n---Sequential tests Succesful---\n\n");
    shutdown();
}