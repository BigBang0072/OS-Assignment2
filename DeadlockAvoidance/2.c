#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#define n 8
#define m 4

int max[n][m] = {
    {1,1,1,0},
    {0,1,1,1},
    {1,0,1,1},
    {1,1,0,1},
    {1,0,0,0},
    {0,1,0,0},
    {0,0,1,0},
    {0,0,0,1}
};
int available[m];
int allocation[n][m] = {
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0}
};

int need[n][m];
int k;
sem_t mutex;
// sem_t mutex2;


bool sufficientResources(int i){
    // sem_wait(&mutex);
    for (int j = 0; j < m; j++) {
        if ( need[i][j] > available[j] ) {
            // sem_post(&mutex);
            return false;
        }
    }
    for (int j = 0; j < m; j++) {
        allocation[i][j] += need[i][j];
        available[j] -= need[i][j];
        need[i][j] = 0;
    }
    // sem_post(&mutex);
    return true;
}

void runProcess(int i){
    printf("Process %d is running\n", i);
    return;
}

void collectResources(int i){
    // sem_wait(&mutex);
    printf("Collecting resources from process %d\n", i);
    for (int j = 0; j < m; j++) {
        available[j] += allocation[i][j];
        allocation[i][j] = 0;
        need[i][j] = max[i][j];
    }
    // sem_post(&mutex);
    return;
}

void* processCode(void* param){
    int iteration = 0;
    int i = *((int*)param);
    printf("Process: %d\n", i);
    while (iteration < k) {
        sem_wait(&mutex);
        bool result = sufficientResources(i);
        while ( result == false );
        printf("Execution of process %d is starting\n", i);
        runProcess(i);
        collectResources(i);
        printf("%d\n", iteration);
        iteration++;
        sem_post(&mutex);
    }
    pthread_exit(0);
}

int main(int argc, char const *argv[]) {
    int a, b, c, d;
    sem_init(&mutex, 0, 1);
    scanf("%d", &k);
    scanf("%d %d %d %d", &a, &b, &c, &d);
    available[0] = a, available[1] = b, available[2] = c, available[3] = d;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
    pthread_t processArray[n];
    for (int i = 0; i < n; i++) {
        pthread_t processArray[i];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        int err = pthread_create(&processArray[i], &attr, processCode, &i);
        if (err == 0) {
            printf("Error in creating thread %d\n", i);
        }
        pthread_join(processArray[i],NULL);
    }
    return 0;
}
