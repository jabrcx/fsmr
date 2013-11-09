/*

Copyright (c) 2013, John A. Brunelle
All rights reserved.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <libdftw.h>

#include "fsmr.h"

//the map function
//see ftw(3) man page (dftw is basically identical)
static int map(const char *fpath, const struct stat *sb, int tflag, void *kv) {
	off_t size;
	uid_t uid;

	switch (tflag) {
		case FTW_D:
			//fpath is a directory
			return 0;
		case FTW_DNR:
			//fpath is a directory which can't be read
			fprintf(stderr, "unreadable directory: %s\n", fpath);
			return 1;
		case FTW_NS:
			//the stat(2) call failed on fpath, which is not a symbolic link
			fprintf(stderr, "unstatable file: %s\n", fpath);
			return 0;
		default: {
			//(FTW_F)
			//ignore symlinks
			if (!S_ISLNK(sb->st_mode)) {
				size = sb->st_size;
				uid  = sb->st_uid;
				//emit a {"uid":size} key/value
				MR_kv_add(kv, (char*)&uid, sizeof(uid_t), (char*)&size, sizeof(off_t));
			}
			return 0;
		}
	}
}

//the reduce function
//see http://mapreduce.sandia.gov/doc/reduce.html
static void reduce(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes) {
	uid_t uid;
	uint64_t totalsize = 0;

	uid = *(uid_t *)key;

	//loop over and sum all values for this key
	int i;
	int offset = 0;
	for (i=0; i<nvalues; i++) {
		totalsize += *(uint64_t *)(multivalue+offset);
		offset += valuebytes[i];
	}
	printf("uid %d: %ld bytes\n", uid, totalsize);
}


int main(int argc, char **argv) {
	if(argc!=2) {
		fprintf(stderr, "usage: %s DIRECTORY\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Walking with root as: `%s'\n", argv[1]);

	if (fsmr(argv[1], map, reduce)) {
		fprintf(stderr, "*** ERROR *** %s failed\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
