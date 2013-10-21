/*

Copyright (c) 2013, John A. Brunelle
All rights reserved.

*/


#ifndef FSMR_H
#define FSMR_H

#include <sys/stat.h>

typedef int (*map_cb_t)(const char* fpath, const struct stat* sb, int typeflag, void *kv);
typedef void (*reduce_cb_t)(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes);

int fsmr(const char *dirpath, map_cb_t map, reduce_cb_t reduce);

#endif //FSMR_H
