define TEST_INSTRUCTIONS
double check output against:
	find /opt/mrmpi-2013-09-17 -type f | wc -l

double check total size against:
	find /opt/mrmpi-2013-09-17 -type f | xargs stat -c%s | awk '{sum+=$$1} END {print sum}'

---

endef
export TEST_INSTRUCTIONS


cmapreduce_extra.o: cmapreduce_extra.cpp
	mpic++ -g -O -c cmapreduce_extra.cpp

fsmr.o: fsmr.c
	mpicc -g -O -c fsmr.c

fsmr: cmapreduce_extra.o fsmr.o
	mpic++ -g -O -o fsmr cmapreduce_extra.o fsmr.o -lmrmpi -ldftw


test: fsmr
	@echo "$$TEST_INSTRUCTIONS"
	mpirun -np 3 ./fsmr /opt/mrmpi-2013-09-17


all: fsmr

clean:
	rm -f fsmr *.o *.stdout *.stderr output.*
