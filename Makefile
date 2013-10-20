fsmr_pure_mrmpi: fsmr_pure_mrmpi.c
	mpicc -g -O -c fsmr_pure_mrmpi.c
	mpic++ -g -O -o fsmr_pure_mrmpi fsmr_pure_mrmpi.o -lmrmpi

fsmr_hybrid_dftw_mrmpi: fsmr_pure_mrmpi.c
	mpicc -g -O -c fsmr_hybrid_dftw_mrmpi.c
	mpic++ -g -O -o fsmr_hybrid_dftw_mrmpi fsmr_hybrid_dftw_mrmpi.o -lmrmpi -ldftw

test_pure_mrmpi: fsmr_pure_mrmpi
	$(info double check output against:)
	$(info - find /opt/mrmpi-2013-09-17 -type f | wc -l)
	$(info double check total size against:)
	$(info - find /opt/mrmpi-2013-09-17 -type f | xargs stat -c%s | awk '{sum+=$$1} END {print sum}')
	$(info)
	mpirun -np 3 ./fsmr_pure_mrmpi /opt/mrmpi-2013-09-17

test_hybrid_dftw_mrmpi: fsmr_hybrid_dftw_mrmpi
	$(info double check output against:)
	$(info - find /opt/mrmpi-2013-09-17 -type f | wc -l)
	$(info double check total size against:)
	$(info - find /opt/mrmpi-2013-09-17 -type f | xargs stat -c%s | awk '{sum+=$$1} END {print sum}')
	$(info)
	mpirun -np 3 ./fsmr_hybrid_dftw_mrmpi /opt/mrmpi-2013-09-17

all: fsmr_pure_mrmpi fsmr_hybrid_dftw_mrmpi

clean:
	rm -f fsmr_pure_mrmpi fsmr_hybrid_dftw_mrmpi *.stdout *.stderr
