#include <stdio.h>
#include <stdlib.h>

#include <libdftw.h>
#include <cmapreduce.h>
#include "fsmr.h"


static int map(const char *fpath, const struct stat *sb, int tflag, void *kv) {
	return 0;
}

static void reduce(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes) {
}


int main(int argc, char **argv) {
	if(argc!=2) {
		fprintf(stderr, "usage: %s DIRECTORY\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "recursing directory: %s\n", argv[1]);

	if (fsmr(argv[1], map, reduce)) {
		fprintf(stderr, "*** ERROR *** %s failed\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
