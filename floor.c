#include "elevator.h"

int main(int argc,char* argv[]){

	/*map the shared memory */
	int shared_seg_size = sizeof(struct shared_data);
	int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmfd, shared_seg_size);
	struct shared_data* shared_msg = (struct shared_data*)
		mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);

	/*semaphore*/
	sem_t* sem_id = sem_open(SEM_PATH, 0);

	struct shared_data in_msg;

	sem_wait(sem_id);

	memcpy(&in_msg, shared_msg, sizeof(struct shared_data));

	sem_post(sem_id);

	printf("%d %d %d\n",in_msg.direction,in_msg.status,in_msg.floor);
	for(int i=0;i<3;i++){
		printf("%d %d %d\n",in_msg.up[i],in_msg.down[i],in_msg.dst[i]);
	}


	shm_unlink(SHMOBJ_PATH);
	sem_close(sem_id);
	sem_unlink(SEM_PATH);
	return 0;
}
