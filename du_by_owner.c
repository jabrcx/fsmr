/*

Copyright (c) 2013, John A. Brunelle
All rights reserved.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <libdftw.h>

#include "fsmr.h"

//the keys (easier than using strings "count" and "size")
static char count_key = 1;
static char size_key = 2;

static int map(const char *fpath, const struct stat *sb, int tflag, void *kv) {
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
				MR_kv_add(kv, &size_key, 1, (char*)&size, sizeof(off_t));
				
				//the value here in theory is always 1, so don't even bother, just use NULL
				MR_kv_add(kv, &count_key, 1, NULL, 0); 
			}
			return 0;
		}
	}
}

static void reduce(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes) {
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
	if(argc!=2) {
		fprintf(stderr, "usage: %s DIRECTORY\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Walking with root as: `%s'\n", argv[1]);

	if (fsmr(argv[1], map, reduce)) {
		fprintf(stderr, "*** ERROR *** fsmr failed\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
