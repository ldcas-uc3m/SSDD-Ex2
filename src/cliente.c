#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib/claves.h"

#define MAX_VALUE1 256


int main() {
    if (init() != 0) {
        printf("Error in init\n");
        return -1;
    }

    int key = 0;
    char* value1 = "Prueba";
    int value2 = 1;
    double value3 = 2.4;  
    int err;

    err = set_value(key, value1, value2, value3);
    if (err!=0){
        printf("Error while inserting the value\n");
        return -1;
    }

    int key1 = 0;
    err = exist(key1);
    if (err==-1){
        printf("Error in exist\n");
        return -1;
    }


    int newKey = 1;
    err=copy_key(key1, newKey);
    if (err!=0){
        printf("Error in copy_key\n");
        return -1;
    }

    int value2Modified = 2;
    err = modify_value(newKey, value1, value2Modified, value3);
    if (err!=0){
        printf("Error in modify_value\n");

        return -1;
    }
    
    char* value1Get = malloc(MAX_VALUE1 * sizeof(char));
    int value2Get;
    double value3Get;
    err = get_value(newKey, value1Get, &value2Get, &value3Get);
    if (err==0){
        printf("The obtained values are %s , %d, %f\n", value1Get, value2Get, value3Get);
    }else{
        free(value1Get);
        printf("Error in get_value\n");
        return -1;
    }

    return 0;
}