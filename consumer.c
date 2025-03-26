// consumer.c
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

void *consume(void *arg) {
    int item_index = 0;
    while (1) {
        sleep(2);
        sem_wait(full);
        sem_wait(mutex);

        int item = table[item_index % TABLE_SIZE];
        printf("[Consumer] Consumed item %d\n", item);
        table[item_index % TABLE_SIZE] = -1;
        item_index++;

        sem_post(mutex);
        sem_post(empty);
    }
    return NULL;
}

int main() {
    init = sem_open(SEM_INIT, 0);
    if (init == SEM_FAILED) {
        perror("sem_open init");
        exit(1);
    }
    sem_wait(init); // Wait for producer to signal readiness

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(1);
    }

    table = mmap(0, TABLE_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    mutex = sem_open(SEM_MUTEX, 0);
    if (mutex == SEM_FAILED) {
        perror("sem_open mutex");
        exit(1);
    }

    empty = sem_open(SEM_EMPTY, 0);
    if (empty == SEM_FAILED) {
        perror("sem_open empty");
        exit(1);
    }

    full = sem_open(SEM_FULL, 0);
    if (full == SEM_FAILED) {
        perror("sem_open full");
        exit(1);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, consume, NULL);
    pthread_join(tid, NULL);

    return 0;
}

