#include <stdlib.h>
#include <stdio.h>

#include <libdftw.h>


static int map(const char *fpath, const struct stat *sb, int tflag) {
	return 0;
}


int main(int argc, char **argv) {
	if(argc!=2) {
		fprintf(stderr, "usage: %s DIRECTORY\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "recursing directory: %s\n", argv[1]);
	
	if (dftw(argv[1], map)) {
		fprintf(stderr, "*** ERROR *** %s failed\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
