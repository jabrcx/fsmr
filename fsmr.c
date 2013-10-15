/*

Example showing basic usage of MapReduce-MPI.

This example uses the MR_map form where the argument is directory (or multiple 
directories) to recurse.  Specifically, in this case, usage is:

	mrmpi_ftw_count_and_du DIRECTORY

If it fails to stat any files along the way, it writes a message about it to 
stderr, but otherwise keeps moving on, excluding it from the results, and will 
still exit with status 0 if nothing else goes wrong.  Such behavior would not 
be desired in a production program!

This counts files and sums their sizes.  Map is like:

	emit {"count": 1}, {"size": sb.st_size}

and reduce is like:

	sum all values for each of the two unique keys, "count" and "size"

Note that since all the counts and sizes are in a single key, respectively, the 
reduce is restricted to just one processor for each (and possibly one overall, 
since this makes no attempt to ensure the hashing puts them on separate 
processors).

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
#include <cmapreduce.h>


//the keys (easier than using strings "count" and "size")
static char count_key = 1;
static char size_key = 2;


void mymap(int itask, char *filename, void *kv, void *ptr) {
	struct stat sb;
	off_t size;
	if (lstat(filename, &sb)) {
		fprintf(stderr, "*** ERROR *** unable to stat [%s]: ", filename);
		perror("");
		return;
	}

	//ignore symlinks
	if (!S_ISLNK(sb.st_mode)) {
		size = sb.st_size;
		MR_kv_add(kv, &size_key, 1, (char*)&size, sizeof(off_t));
		
		//the value here in theory is always 1, so don't even bother, just use NULL
		MR_kv_add(kv, &count_key, 1, NULL, 0); 
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

	void *mr = MR_create(*mrcomm);
	MR_map_file(mr, argc-1, &argv[1], 0, 1, 0, &mymap, NULL);
	MR_collate(mr, NULL);
	MR_reduce(mr, &myreduce, NULL);
	MR_destroy(mr);

	MPI_Comm_free(mrcomm);
	MPI_Finalize();
	free(mrcomm);
	
	exit(EXIT_SUCCESS);
}
