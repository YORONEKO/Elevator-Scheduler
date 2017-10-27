#include "elevator.h"

struct elevator_status status = {UP,STOP,1};

struct elevator_status* shared_memory;
sem_t* sem_id;


int main(int argc,char* argv[]){
	/* Map the shared memory */
	int shared_seg_size = sizeof(struct elevator_status);
	int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmfd, shared_seg_size);
	shared_memory = (struct elevator_status*)
		mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

	/* semaphore */
	sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);

	/* Initial the shared data */
	sem_wait(sem_id);
	memcpy(shared_memory, &status, sizeof(struct elevator_status));
	sem_post(sem_id);

	/* Initial the message queue */
	int msgid = msgget((key_t)MSG_KEY, 0666 | IPC_CREAT);
	if(msgid == -1){
		fprintf(stderr, "msgget failed with error: %d\n", errno);  
        exit(EXIT_FAILURE);  
	}


	/* receive the messages */
	/*
	* struct msg_st data;
	* int msg_type = 0;
	* msgrcv(msgid, (void*)&data,sizeof(int),msg_type, 0)
	* if return -1, error
	*/
	struct msg_st data;
	int msg_type = 0;
	while(1){
		msgrcv(msgid, (void*)&data,sizeof(int),msg_type, 0);
		printf("%d\n",data.val);
	}


	/* TODO wait for testing */
	sleep(10);

	/* delete the message queue */
	if(msgctl(msgid, IPC_RMID, 0) == -1) {  
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");  
        exit(EXIT_FAILURE);  
    }  

	shm_unlink(SHMOBJ_PATH);
	sem_close(sem_id);
	sem_unlink(SEM_PATH);
	return 0;
}
