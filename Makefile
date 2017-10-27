elevator : elevator.h elevator.c
	gcc -o elevator elevator.h elevator.c -lm -lrt -pthread
controller : elevator.h elevator.c                                                 │3
	gcc -o controller elevator.h controller.c -lm -lrt -pthread     