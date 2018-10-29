#include <stdio.h>
#include <stdlib.h>
#include "declaration.h"

//Global Variables
#define max_process 50
Process process_table[max_process];//will have the array for all the processes
int CPU_HOLDER=-1;//which process is holding the current CPU

int main(){
    //Testing the push and pop operation
    Event e1,e2,e3,*event_heap[10];
    Event *e;

    int size=-1;
    e1.time=15;
    e2.time=12;
    e3.time=18;
    add_and_min_heapify(&size,&e1,event_heap);
    add_and_min_heapify(&size,&e2,event_heap);
    add_and_min_heapify(&size,&e3,event_heap);

    printf("%d\n",size);
    e=pop_and_min_heapify(&size,event_heap);
    printf("%d %d\n",e->time,size);
    e=pop_and_min_heapify(&size,event_heap);
    printf("%d %d\n",e->time,size);
    e=pop_and_min_heapify(&size,event_heap);
    printf("%d %d\n",e->time,size);
    e=pop_and_min_heapify(&size,event_heap);
    if(e!=NULL){
        printf("%d %d\n",e->time,size);
    }
    return 0;
}
