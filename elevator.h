#ifndef __EVEVATOR_
#define __EVEVATOR_

#include <stdio.h>                                                              
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/msg.h>  
#include <errno.h>

#define SHMOBJ_PATH "/shm_AOS"
#define SEM_PATH "/sem_AOS"

#define UP(x) (x*10+1)
#define DOWN(x) (x*10+2)
#define FLOOR(x) (x*10)
#define MSG_KEY 1234

enum DIRECTION {
	DIR_UP, DIR_DOWN 
};

enum STATUS{
	RUN, STOP
};

struct elevator_status {
	enum DIRECTION direction;
	enum STATUS status;
	int floor;
};

struct msg_st{
	long int type;
	int val;
};

#endif

