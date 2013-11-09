/*

Copyright (c) 2013, John A. Brunelle
All rights reserved.

*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <mpi.h>
#include <libdftw.h>
#include <cmapreduce.h>

#include "cmapreduce_extra.h"
#include "fsmr.h"

//handles for the callbacks
map_cb_t map_cb;
reduce_cb_t reduce_cb;

//the MapReduce "object"
void *mrg;

//the KeyValue "object" (the one in mrg)
void *kvg;

//wrapper for the exact callback form that dftw expects
static int _map_wrap(const char *fpath, const struct stat *sb, int typeflag) {
	return map_cb(fpath, sb, typeflag, kvg);
}

//wrapper for the exact callback form that MR_reduce expects
static void _reduce_wrap(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, void *kv, void *ptr) {
	reduce_cb(key, keybytes, multivalue, nvalues, valuebytes);
}

int fsmr(const char *dirpath, map_cb_t map, reduce_cb_t reduce) {
	//set the callback handles
	map_cb = map;
	reduce_cb = reduce;

	//MPI_Init if necessary
	int finalize = 0;
	int mpi_initialized;
	if (MPI_Initialized(&mpi_initialized)!=MPI_SUCCESS) {
		fprintf(stderr, "*** ERROR *** unable to test if MPI initialized\n");
		return -1;
	}
	if (!mpi_initialized) {
		if (MPI_Init(NULL, NULL)!=MPI_SUCCESS) {
			fprintf(stderr, "*** ERROR *** unable to initialize MPI\n");
			return -1;
		}
		finalize = 1;
	}

	//use a separate communicator
	MPI_Comm *mrcomm = (MPI_Comm*)malloc(sizeof(MPI_Comm));
	MPI_Comm_dup(MPI_COMM_WORLD, mrcomm);
	MPI_Comm_set_name(*mrcomm, "MapReduce-MPI Comm");

	//init
	mrg = MR_create(*mrcomm);

	//map
	MR_open(mrg);
	kvg = MR_get_kv(mrg);  //this is custom add-on; see cmapreduce_extra.cpp
	dftw(dirpath, _map_wrap);
	MR_close(mrg);

	//shuffle
	MR_collate(mrg, NULL);

	//reduce
	MR_reduce(mrg, &_reduce_wrap, NULL);

	//finalize
	MR_destroy(mrg);
	MPI_Comm_free(mrcomm);
	if (finalize) {
		MPI_Finalize();
	}
	free(mrcomm);

	return 0;
}
