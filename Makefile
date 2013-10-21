define TEST_INSTRUCTIONS

double check output against:
	find /opt/mrmpi-2013-09-17 -type f | wc -l

double check total size against:
	find /opt/mrmpi-2013-09-17 -type f | xargs stat -c%s | awk '{sum+=$$1} END {print sum}'

endef
export TEST_INSTRUCTIONS


cmapreduce_extra.o: cmapreduce_extra.cpp
	mpic++ -g -O -c cmapreduce_extra.cpp

fsmr.o: fsmr.c
	mpicc -g -O -c fsmr.c

du_by_owner.o: du_by_owner.c
	mpicc -g -O -c du_by_owner.c


du_by_owner: cmapreduce_extra.o fsmr.o du_by_owner.o
	mpic++ -g -O -o du_by_owner cmapreduce_extra.o fsmr.o du_by_owner.o -lmrmpi -ldftw


test: du_by_owner
	@echo "$$TEST_INSTRUCTIONS"
	mpirun -np 3 ./du_by_owner /opt/mrmpi-2013-09-17


all: du_by_owner

clean:
	rm -f du_by_owner *.o *.stdout *.stderr output.*
