#
# Copyright (c) 2013, John A. Brunelle
# All rights reserved.
#


#--config

CC = mpicc
#CFLAGS += -g -fPIC -I...
#LDFLAGS += -L...


#--- building

PROG = example

all: $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): $(PROG).o
	$(CC) $(LDFLAGS) $< -ldftw -o $@


#--- testing

define TEST_INSTRUCTIONS
endef
export TEST_INSTRUCTIONS

test_args:
ifndef TEST_ARGS
		$(warning `make test' requires the TEST_ARGS environment variable be set.)
		$(warning Set it to the path of a directory to walk, for example:)
		$(warning export TEST_ARGS='/opt')
		$(error You must set the TEST_ARGS environment variable.)
endif

test: $(PROG) test_args
	@echo "$$TEST_INSTRUCTIONS"
	mpirun -np 3 --mca btl self,tcp ./$(PROG) $(TEST_ARGS)


#--- cleaning

clean:
	rm -f $(PROG) *.o *.so
