elevator : elevator.h elevator.c
	gcc -o elevator elevator.h elevator.c -lm -lrt -pthread