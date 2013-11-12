#
# Copyright (c) 2013, John A. Brunelle
# All rights reserved.
#


CC = mpicc
CPP = mpic++
CFLAGS += -g -O0 -fPIC -I..
CPPFLAGS += -g -O0 -fPIC
LDFLAGS += -L..

PROGS = du_by_owner


define TEST_INSTRUCTIONS

double check total size against:
	find $(TEST_ARGS) -type f | xargs stat -c%s | awk '{sum+=$$1} END {print sum}'

endef
export TEST_INSTRUCTIONS


all: $(PROGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

du_by_owner: du_by_owner.o
	$(CPP) $(CPPFLAGS) $(LDFLAGS) $< -lfsmr -ldftw -lmrmpi -o $@

test_args:
ifndef TEST_ARGS
		$(warning `make test' requires the TEST_ARGS environment variable be set.)
		$(warning Set it to the path of a directory to walk, for example:)
		$(warning export TEST_ARGS='/opt')
		$(error You must set the TEST_ARGS environment variable.)
endif

test: $(PROGS) test_args
	@echo "$$TEST_INSTRUCTIONS"
	LD_LIBRARY_PATH=..:"$$LD_LIBRARY_PATH" mpirun -np 3 ./du_by_owner $(TEST_ARGS)

clean:
	rm -f $(PROGS) *.o *.stdout *.stderr output.*
