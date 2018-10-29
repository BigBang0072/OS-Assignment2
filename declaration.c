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

// Defining the file reading function
int read_process_file(int process_times[][2],char filename[]){
    //Opening the filepointer
    FILE *fp;
    printf("Reading the file\n");
    fp=fopen(filename,"r");

    //Handling bad file descriptor
    if(fp==NULL){
        printf("Error Reading file\n");
        exit(0);
    }
    //Reading the file
    int i=0,j=0,num=0;
    char ch;
    while((ch=fgetc(fp)) != EOF){
        if(ch=='\n'){
            process_times[i][j]=num;
            num=0;
            j=0;
            i++;
        }
        else if(ch==','){
            process_times[i][j]=num;
            j++;
            num=0;
        }
        else{
            num=num*10+(ch-'0');
        }
    }
    //Sorting the array in ascending order
    bubble_sort(i,process_times);
    for(int k=0;k<i;k++){
        printf("%d %d\n",process_times[k][0],process_times[k][1]);
    }
    return i;//return the size upto which process arrival is there
}

void bubble_sort(int size,int process_times[][2]){
    for(int i=0;i<size-1;i++){
        for(int j=0;j<size-1-i;j++){
            int temp0=process_times[j][0];
            int temp1=process_times[j][1];
            if(process_times[j][0]==process_times[j+1][0]){
                if(process_times[j][1]>process_times[j+1][1]){
                    //Swapping the elements
                    process_times[j][0]=process_times[j+1][0];
                    process_times[j][1]=process_times[j+1][1];

                    process_times[j+1][0]=temp0;
                    process_times[j+1][1]=temp1;
                }
            }
            else if(process_times[j][0]>process_times[j+1][0]){
                //Swapping the elements
                process_times[j][0]=process_times[j+1][0];
                process_times[j][1]=process_times[j+1][1];

                process_times[j+1][0]=temp0;
                process_times[j+1][1]=temp1;
            }
        }
    }
}

//Adding the Queue related functionality
Queue* push_to_queue(int pid,Queue *tail){
    Queue *qu=(Queue*)malloc(sizeof(Queue));
    qu->pid=pid;
    qu->next=NULL;

    //Now linking the new element to NULL
    if(tail!=NULL){
        tail->next=qu;
        tail=qu;
    }
    return qu;
}
Queue* pop_from_queue(Queue *head){
    //handling base case
    if(head==NULL){
        return NULL;
    }
    Queue *new_head=head->next;
    free(head);
    return new_head;
}
