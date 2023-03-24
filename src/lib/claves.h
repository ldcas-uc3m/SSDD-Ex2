/*
Definición de las operaciones del cliente
*/

#ifndef _CLAVES_H_
#define _CLAVES_H_
#define PREFIX ClienteNumero_



int init(void);
int set_value(int key, char* value1, int value2, double value3);
int get_value(int key, char* value1, int* value2, double* value3);
int modify_value(int key, char* value1, int value2, double value3);
int exist(int key);
int copy_key(int key1, int key2);
int shutdown(void);

#endif