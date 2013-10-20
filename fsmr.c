/*

Copyright (c) 2013, John A. Brunelle
All rights reserved.

*/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <mpi.h>
#include <libdftw.h>
#include <cmapreduce.h>
#include <cmapreduce_extra.h>


//the keys (easier than using strings "count" and "size")
static char count_key = 1;
static char size_key = 2;

static void *mrg;
static void *kvg;


static int mymap(const char *fpath, const struct stat *sb, int tflag) {
	off_t size;
	
	switch (tflag) {
		case FTW_D:
			return 0;
		case FTW_DNR:
			fprintf(stderr, "unreadable directory: %s\n", fpath);
			return 1;
		case FTW_NS:
			fprintf(stderr, "unstatable file: %s\n", fpath);
			return 0;
		default: {
			//ignore symlinks
			if (!S_ISLNK(sb->st_mode)) {
				size = sb->st_size;
				MR_kv_add(kvg, &size_key, 1, (char*)&size, sizeof(off_t));
				
				//the value here in theory is always 1, so don't even bother, just use NULL
				MR_kv_add(kvg, &count_key, 1, NULL, 0); 
			}
			return 0;
		}
	}
}

void myreduce(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, void *kv, void *ptr) {
	if (*key==count_key) {
		printf("total path count: %d\n", nvalues);
	} else if (*key==size_key) {
		uint64_t totalsize = 0;

		int i;
		int offset = 0;
		for (i=0; i<nvalues; i++) {
			totalsize += *(uint64_t *)(multivalue+offset);
			offset += valuebytes[i];
		}
		printf("total path size: %ld\n", totalsize);
	}
}


int main(int argc, char **argv) {
	int myrank, nranks;

	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		fprintf(stderr, "*** ERROR *** unable to initialize MPI\n");
		exit(EXIT_FAILURE);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nranks);
	if (argc<2) {
		if (myrank==0) fprintf(stderr, "usage: %s PATH...\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

	MPI_Comm *mrcomm = (MPI_Comm*)malloc(sizeof(MPI_Comm));
	MPI_Comm_dup(MPI_COMM_WORLD, mrcomm);
	MPI_Comm_set_name(*mrcomm, "MapReduce-MPI Comm");

	mrg = MR_create(*mrcomm);

	MR_open(mrg);
	kvg = MR_get_kv(mrg);
	dftw(argv[1], mymap);
	MR_close(mrg);

	MR_collate(mrg, NULL);
	MR_reduce(mrg, &myreduce, NULL);
	MR_destroy(mrg);

	MPI_Comm_free(mrcomm);
	MPI_Finalize();
	free(mrcomm);
	
	exit(EXIT_SUCCESS);
}
