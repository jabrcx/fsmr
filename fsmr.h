/*

Copyright (c) 2013, John A. Brunelle
All rights reserved.

*/


#ifndef FSMR_H
#define FSMR_H

#include <sys/stat.h>

//map callback signature -- same as dftw callback, except extra mrmpi KeyValue passed
typedef int (*map_cb_t)(const char* fpath, const struct stat* sb, int typeflag, void *kv);

//reduce callback signature -- same as MR_reduce callback, except without arguments not applicable here
typedef void (*reduce_cb_t)(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes);

//the main call
int fsmr(const char *dirpath, map_cb_t map, reduce_cb_t reduce);

#endif //FSMR_H
