#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <errno.h>
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
sem_t mutex1;
sem_t mutex2;
sem_t mutex3;
time_t t;

bool sufficientResources(int i){
    sem_wait(&mutex2);
    for (int j = 0; j < m; j++) {
        if ( need[i][j] > available[j] ) {
            sem_post(&mutex2);
            return false;
        }
    }
    for (int j = 0; j < m; j++) {
        allocation[i][j] += need[i][j];
        available[j] -= need[i][j];
        need[i][j] = 0;
    }
    sem_post(&mutex2);
    return true;
}

void runProcess(int i){
    printf("Process %d is running\n", i);
    return;
}

void collectResources(int i){

    sem_wait(&mutex3);
    printf("Collecting resources from process %d\n", i);
    while (allocation[i][0] > 0 || allocation[i][1] > 0 || allocation[i][2] > 0 || allocation[i][3] > 0) {
        if ( allocation[i][0] > 0 ) {
            if (rand()%100 <= 66) {
                available[0] += allocation[i][0];
                allocation[i][0] = 0;
                need[i][0] = max[i][0];
                printf("Resource A collected from process %d\n", i);
            }
        }
        if ( allocation[i][1] > 0 ) {
            if (rand()%100 <= 75) {
                available[1] += allocation[i][1];
                allocation[i][1] = 0;
                need[i][1] = max[i][1];
                printf("Resource B collected from process %d\n", i);
            }
        }
        if ( allocation[i][2] > 0 ) {
            if (rand()%100 <= 60) {
                available[2] += allocation[i][2];
                allocation[i][2] = 0;
                need[i][2] = max[i][2];
                printf("Resource C collected from process %d\n", i);
            }
        }
        if ( allocation[i][3] > 0 ) {
            if (rand()%100 <= 66) {
                available[3] += allocation[i][3];
                allocation[i][3] = 0;
                need[i][3] = max[i][3];
                printf("Resource D collected from process %d\n", i);
            }
        }
    }
    sem_post(&mutex3);
    printf("Finished execution of process %d\n", i);
    return;
}

void* processCode(void* param){

    sem_wait(&mutex1);
    int i = *((int*)param);
    bool result = sufficientResources(i);
    while ( result == false );
    printf("Execution of process %d is starting\n", i);
    runProcess(i);
    collectResources(i);
    sem_post(&mutex1);
    pthread_exit(0);
}

int main(int argc, char const *argv[]) {
    int a, b, c, d;
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&mutex3, 0, 1);
    srand((unsigned) time(&t));
    scanf("%d", &k);
    scanf("%d %d %d %d", &a, &b, &c, &d);
    available[0] = a, available[1] = b, available[2] = c, available[3] = d;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
    int processId[n];
    for (int i = 0; i < n; i++) {
        processId[i] = i;
    }
    pthread_t processArray[n];
    for (int i = 0; i < n; i++) {
        processArray[i] = i;
        // pthread_attr_t attr;
        // pthread_attr_init(&attr);
        int iteration = 0;
        while (iteration < k) {
            int err = pthread_create(&processArray[i], NULL, processCode, &processId[i]);
            if (err != 0) {
                printf("Error in creating thread %d\n", i);
            } else {
                // printf("Thread %d created\n", i);
            }
            iteration++;
        }
    }
    for (int i = 0; i < n; i++) {
        pthread_join(processArray[i],NULL);
    }
    return 0;
}
