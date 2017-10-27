#include "elevator.h"

struct elevator_status* shared_memory;
sem_t* sem_id;

int main(){
    /* Map the shared memory */
	int shared_seg_size = sizeof(struct elevator_status);
	int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRUSR);
	ftruncate(shmfd, shared_seg_size);
	shared_memory = (struct elevator_status*)
		mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);

	/* semaphore */
    sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    
    /* message queue */
    int msgid = msgget((key_t)MSG_KEY, 0666 | IPC_CREAT);
    if(msgid == -1)  {  
        fprintf(stderr, "msgget failed with error: %d\n", errno);  
        exit(EXIT_FAILURE);  
    }

    struct msg_st data;
    data.type = 4;
    data.val = 10;
    
    if(msgsnd(msgid,(void*)&data,sizeof(int),0) == -1){
        fprintf(stderr,"msgsnd failed\n");
        exit(EXIT_FAILURE);
    }

    struct elevator_status status;

    /* read the shared memory */
	sem_wait(sem_id);
    memcpy(&status, shared_memory, sizeof(struct elevator_status));
    printf("%d %d %d\n",status.direction,status.status,status.floor);
    sem_post(sem_id);
    
    shm_unlink(SHMOBJ_PATH);
	sem_close(sem_id);
	sem_unlink(SEM_PATH);
}