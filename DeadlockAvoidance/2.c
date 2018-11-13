#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <errno.h>

// n : number of processes
// m : number of resources
#define n 8
#define m 4

// max resources required for each processes
int max[n][m] = {
    {1,1,1,0},//0
    {0,1,1,1},
    {1,0,1,1},//2
    {1,1,0,1},
    {1,0,0,0},//4
    {0,1,0,0},
    {0,0,1,0},//6
    {0,0,0,1}
};

// array for quantity of each resource
int available[m];

// array to store quantities of resources allocated to each process
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

// additional resources required for processes
int need[n][m];
int k;
sem_t mutex1;
sem_t mutex2;
sem_t mutex3;
sem_t mutex_array[n];
time_t t;

// check if resources are available for the process
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

// the body of the process
void runProcess(int i){
    sem_wait(&mutex1);
    printf("Process %d is running\n", i);
    sem_post(&mutex1);
    return;
}

// collect resources according to the collection probabilities
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

// code to run the created thread
void* processCode(void* param){

    int i = *((int*)param);
    sem_wait(&mutex_array[i]);
    //bool result = sufficientResources(i);
    while ( sufficientResources(i) == false );
    printf("Execution of process %d is starting\n", i);
    runProcess(i);
    collectResources(i);
    sem_post(&mutex_array[i]);
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
        sem_init(&mutex_array[i],0,1);
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
