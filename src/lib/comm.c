
#include "comm.h"

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