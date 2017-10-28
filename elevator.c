#include "elevator.h"
#include <pthread.h>

struct elevator_status status = { DIR_UP,STOP,1 };

struct elevator_status* shared_memory;
sem_t* sem_id;
sem_t* w;
sem_t* wrt;

/* in thread */
pthread_mutex_t mutex;
int up[3] = { 0,0,0 };
int down[3] = { 0,0,0 };
int dst[3] = { 0,0,0 };
int count = 0;

void writeIntoStatus() {
	sem_wait(wrt);
	memcpy(shared_memory, &status, sizeof(status));
	sem_post(wrt);
}

void* dealWithMessage() {
	/* Initial the message queue */
	int msgid = msgget((key_t)MSG_KEY, 0666 | IPC_CREAT);
	if (msgid == -1) {
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	/* receive the messages */
	struct msg_st data;
	int msg_type = 0;
	while (1) {
		if (msgrcv(msgid, (void*)&data, sizeof(int), msg_type, 0) == -1) {
			fprintf(stderr, "msgrcv failed with errno: %d\n", errno);
			exit(EXIT_FAILURE);
		}

		/*for test*/
		//printf("%d\n", data.val);

		pthread_mutex_lock(&mutex);
		int count = 0;
		for (int i = 0; i<3; i++) {
			count += (up[i] + down[i] + dst[i]);
		}
		if (data.val == OPEN_DOOR && status.status == STOP) {
			dst[status.floor] = 1;
		}
		else if (data.val != OPEN_DOOR && data.val != CLOSE_DOOR) {
			int dstfloor = data.val / 10;
			if (data.val == UP(dstfloor))
				up[dstfloor - 1] = 1;
			else if (data.val == DOWN(dstfloor))
				down[dstfloor - 1] = 1;
			else
				dst[dstfloor - 1] = 1;
			if (count == 0 && dstfloor != status.floor) {
				status.direction = dstfloor > status.floor ? DIR_UP : DIR_DOWN;
				writeIntoStatus();
			}
		}
		pthread_mutex_unlock(&mutex);
	}

	if (msgctl(msgid, IPC_RMID, 0) == -1) {
		fprintf(stderr, "msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
}
void* dealWithElevator()
{
	struct msg_st data;
	struct msg_st buf[100];
	int len = 0;
	int msg_type = 0;
	//struct elevator_status elevator;
	while (1)
	{
		pthread_mutex_lock(&mutex);

		writeIntoStatus();

		int step = 0;
		int flag = 0;
		int i;

		if (status.floor == GROUND)
		{
			status.direction = UP;
			status.status = STOP;
		}
		else if (status.floor == CELLING)
		{
			status.direction = DOWN;
			status.status = STOP;
		}
		else
		{
			if (status.direction == UP)
			{
				for (i = status.floor; i < CELLING; i++)
				{
					if (up[i] > 0 || dst[i] > 0)
					{
						flag = 1;
						break;
					}
				}
				if (i == status.floor)
					step = 0;
				else
					step = 1;
			}
			else
			{
				for (i = status.floor; i >= GROUND; i--)
				{
					if (down[i] > 0 || dst[i] > 0)
					{
						flag = 2;
						break;
					}
				}
				if (i == status.floor)
					step = 0;
				else
					step = -1;
			}
		}
		pthread_mutex_unlock(&mutex);
		if (flag == 0)
		{
			sleep(2);
		}
		pthread_mutex_lock(&mutex);

		if (flag == 1)
		{
			status.status = RUN;
			status.floor += step;
		}
		else
		{
			status.status = RUN;
			status.floor += step;
		}

		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, char* argv[]) {
	/* Map the shared memory for status */
	int shmid_status = shmget((key_t)STATUS_KEY, 4096, 0666 | IPC_CREAT);
	if (shmid_status == -1) {
		printf("shared memory create failed %d\n", errno);
		perror("shmget");
		exit(EXIT_FAILURE);
	}
	void* shm_status = shmat(shmid_status, (void*)NULL, 0);
	if (shm_status == (void*)-1) {
		printf("shmat failed %d", errno);
		perror("shmat");
		exit(EXIT_FAILURE);
	}
	shared_memory = (struct elevator_status*) shm_status;

	/* semaphore for writer*/
	wrt = sem_open(WRT_KEY, O_CREAT, 0644, 1);
	if (wrt == SEM_FAILED) {
		perror("unable to create semaphore");
		sem_unlink("wrt");
		exit(-1);
	}

	/* for initial */
	w = sem_open(W_KEY, O_CREAT, 0644, 0);
	if (w == SEM_FAILED) {
		perror("unable to create semaphore");
		sem_unlink(W_KEY);
		exit(-1);
	}

	/* Initialize the shared memory */
	writeIntoStatus();
	/* increase 4 times because 4 process need to wait */
	for (int i = 0; i<NR_READER; i++)
		sem_post(w);

	pthread_t mDealer;

	pthread_mutex_init(&mutex, NULL);

	pthread_create(&mDealer, NULL, dealWithMessage, NULL);
	/* TODO add your pthread here */

	pthread_t mElevator;

	pthread_mutex_init(&mutex, NULL);

	pthread_create(&mElevator, NULL, dealWithElevator, NULL);

	/* TODO add join */
	pthread_join(mDealer, NULL);
	pthread_join(mElevator, NULL);
	pthread_mutex_destroy(&mutex);

	/* close the sem */
	sem_close(w);
	sem_close(wrt);
	sem_unlink(W_KEY);
	sem_unlink(WRT_KEY);

	/* close the shared memory */
	if (shmdt(shm_status) == -1) {
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	if (shmctl(shmid_status, IPC_RMID, 0) == -1) {
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}