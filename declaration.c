#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "declaration.h"

//Definiting the min heap
void add_and_min_heapify(int *size,Event *new_eve,Event *event_heap[]){
    //Adding the event to the heap
    *size+=1;   //incrementing the size
    event_heap[*size]=new_eve;

    //Heapifying the array
    int idx=*size;
    while(idx>=1){
        //Retreiving the idx of the parent
        int pidx=(idx-1)/2;
        //Comparing the condition for min-heap
        if(event_heap[idx]->time<event_heap[pidx]->time){
            Event *etemp=event_heap[idx];
            event_heap[idx]=event_heap[pidx];
            event_heap[pidx]=etemp;
            idx=pidx;
        }
        else{
            break;
        }
    }
}

Event* pop_and_min_heapify(int *size,Event *event_heap[]){
    //handling the base case
    if(*size==-1){
        return NULL;
    }
    //Now handling the non-empty heap
    Event *ret_eve=event_heap[0];
    event_heap[0]=event_heap[*size];
    *size-=1;   //decreasing the size of heap

    //Heapyfying the heap
    int idx=0;
    while(idx<*size){
        int cidx1=2*idx+1;
        int cidx2=2*idx+2;
        int min=INT_MAX;
        int mincidx=-1;
        if(cidx1<=*size && event_heap[idx]->time>event_heap[cidx1]->time){
            mincidx=cidx1;
            min=event_heap[cidx1]->time;
        }
        if(cidx2<=*size && min>event_heap[cidx2]->time){
            mincidx=cidx2;
            min=event_heap[cidx2]->time;
        }

        //Now swapping the element if possible
        if(mincidx!=-1){
            Event *temp_eve=event_heap[idx];
            event_heap[idx]=event_heap[mincidx];
            event_heap[mincidx]=temp_eve;
            idx=mincidx;
        }
        else{
            break;
        }
    }

    //Returning the min element
    return ret_eve;
}

// int main(){
//     //Testing the push and pop operation
//     Event e1,e2,e3,*event_heap[10];
//     Event *e;
//
//     int size=-1;
//     e1.time=15;
//     e2.time=12;
//     e3.time=18;
//     add_and_min_heapify(&size,&e1,event_heap);
//     add_and_min_heapify(&size,&e2,event_heap);
//     add_and_min_heapify(&size,&e3,event_heap);
//
//     printf("%d\n",size);
//     e=pop_and_min_heapify(&size,event_heap);
//     printf("%d %d\n",e->time,size);
//     e=pop_and_min_heapify(&size,event_heap);
//     printf("%d %d\n",e->time,size);
//     e=pop_and_min_heapify(&size,event_heap);
//     printf("%d %d\n",e->time,size);
//     e=pop_and_min_heapify(&size,event_heap);
//     if(e!=NULL){
//         printf("%d %d\n",e->time,size);
//     }
//     return 0;
// }
