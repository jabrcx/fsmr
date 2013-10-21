#
# Copyright (c) 2013, John A. Brunelle
# All rights reserved.
#


CC = mpicc
CPP = mpic++
CFLAGS += -g -O0 -fPIC
CPPFLAGS += -g -O0 -fPIC
LDFLAGS +=

all: libfsmr.so

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -o $@ -c $<

libfsmr.so: fsmr.o cmapreduce_extra.o
	$(CPP) $(CPPFLAGS) -shared fsmr.o cmapreduce_extra.o -ldftw -o libfsmr.so

clean:
	rm -f *.o *.so
