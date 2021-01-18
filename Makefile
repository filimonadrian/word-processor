build:
	mpic++ tema3.cpp -o tema3
run:
	mpirun -np 5 ./tema3
test_read:
	g++ read_test.cpp -lpthread -o read
	./read
clear:
	rm tema3
