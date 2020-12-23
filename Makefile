build:
	mpic++ tema3.cpp -o tema3
run:
	mpirun -np 5 ./tema3
clear:
	rm tema3
