#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib/claves.h"

#define MAX_VALUE1 256


int main() {
    init();

    int key = 0;
    char* value1 = "Prueba";
    int value2 = 1;
    double value3 = 2.4;  
    int err;

    err = set_value(key, value1, value2, value3);
    if (err!=0){
        printf("Error while inserting the value");
        return -1;
    }

    int key1 = 0;
    err = exist(key1);
    if (err==0){
        int newKey = 1;
        err=copy_key(key1, newKey);
        if (err!=0){
            return -1;
        }

        int value2Modified = 2;
        err = modify_value(newKey, value1, value2Modified, value3);
        if (err!=0){
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
            return -1;
        }
        
    }else{
        return -1;
    }

    err = shutdown();
    if (err!=0){
        return -1;
    }
    return 0;
}