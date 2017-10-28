elevator : elevator.h elevator.c
	gcc -o elevator elevator.h elevator.c -lm -lrt -pthread
floor1: elevator.h floor1.c
	gcc -o floor1 elevator.h floor1.c -lm -lrt -pthread     
floor2: elevator.h floor2.c
	gcc -o floor2 elevator.h floor2.c -lm -lrt -pthread     
floor3: elevator.h floor3.c
	gcc -o floor3 elevator.h floor3.c -lm -lrt -pthread     
