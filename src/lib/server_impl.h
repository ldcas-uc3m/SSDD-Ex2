/*
Definition of server implementation
*/

#ifndef _SERVER_IMPL_H_
#define _SERVER_IMPL_H_


int init();
int set(int key, char* value1, int value2, double value3);
int get(int key, char* value1, int* value2, double* value3);
int exist(int key);
int modify(int key, char* value1, int value2, double value3);
int copy_key(int key1, int key2);
int delete_key(int key);
void destroy();

#endif