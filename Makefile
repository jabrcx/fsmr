fsmr: fsmr.c
	mpicc -g -O -c fsmr.c
	mpic++ -g -O -o fsmr fsmr.o -lmrmpi -ldftw

test: fsmr
	$(info double check total count against:)
	$(info - find /opt/mrmpi-2013-09-17/include/* | wc -l)
	$(info double check total size against:)
	$(info - find /opt/mrmpi-2013-09-17 -type f | xargs stat -c%s | awk '{sum+=$$1} END {print sum}')
	$(info)
	mpirun -np 3 ./fsmr /opt/mrmpi-2013-09-17

all: fsmr

clean:
	rm -f fsmr *.stdout *.stderr
