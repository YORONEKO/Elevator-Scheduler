# Elevator-Scheduler
## 关于互斥访问

请参考elevator.c中的

``` c
struct elevator_status* shared_memory;
sem_t* sem_id;
/* Map the shared memory */
int shared_seg_size = sizeof(struct elevator_status);
/* only use S_IRUSR when you only need to read */
int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
ftruncate(shmfd, shared_seg_size);
shared_memory = (struct elevator_status*)
  mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

/* semaphore */
sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);

/* modify or read the shared data*/
sem_wait(sem_id);
/* access the data by the pointer shared_memory */
sem_post(sem_id);
```
## 关于Makefile
记得加上链接选项!
你可以直接使用make elevator
