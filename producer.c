// producer.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define SHM_NAME "/prodcons_shm"
#define SEM_MUTEX "/mutex_sem"
#define SEM_FULL "/full_sem"
#define SEM_EMPTY "/empty_sem"
#define SEM_INIT "/init_sem"
#define TABLE_SIZE 2

int *table;
sem_t *mutex, *full, *empty, *init;

void *produce(void *arg) {
    int item = 0;
    while (1) {
        sleep(1);
        sem_wait(empty);
        sem_wait(mutex);

        table[item % TABLE_SIZE] = item;
        printf("[Producer] Produced item %d\n", item);
        item++;

        sem_post(mutex);
        sem_post(full);
    }
    return NULL;
}

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, TABLE_SIZE * sizeof(int)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    table = mmap(0, TABLE_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open mutex");
        exit(1);
    }

    empty = sem_open(SEM_EMPTY, O_CREAT, 0666, TABLE_SIZE);
    if (empty == SEM_FAILED) {
        perror("sem_open empty");
        exit(1);
    }

    full = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    if (full == SEM_FAILED) {
        perror("sem_open full");
        exit(1);
    }

    init = sem_open(SEM_INIT, O_CREAT, 0666, 0);
    if (init == SEM_FAILED) {
        perror("sem_open init");
        exit(1);
    }

    // Signal to consumer that initialization is done
    sem_post(init);

    pthread_t tid;
    pthread_create(&tid, NULL, produce, NULL);
    pthread_join(tid, NULL);

    return 0;
}
