build:
	mpic++ tema3.cpp -o tema3
run:
	mpirun -np 5 ./tema3
test_read:
	g++ read_test.cpp -lpthread -o read
	./read
test_mpi:
	mpic++ mpi_test.cpp -o mpi_test
	mpirun -np 5 ./mpi_test
clear:
	rm tema3
