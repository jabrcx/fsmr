#include <stdlib.h>
#include <stdio.h>

#include <mpi.h>
#include <cmapreduce.h>


void mymap(int itask, char *filename, void *kv, void *ptr) {
}

void myreduce(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, void *kv, void *ptr) {
}


int main(int argc, char **argv) {
	int myrank, nranks;

	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		fprintf(stderr, "*** ERROR *** unable to initialize MPI\n");
		exit(EXIT_FAILURE);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nranks);

	if (argc!=2) {
		if (myrank==0) fprintf(stderr, "usage: %s DIRECTORY\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}
	if (myrank==0) fprintf(stdout, "recursing directory: %s\n", argv[1]);

	//(this could be made simpler by not even using a dedicated communicator)
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
