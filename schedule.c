#include <stdio.h>
#include <stdlib.h>
#include "declaration.h"

//Global Variables
#define max_process 50
#define max_events 1000
Process *process_table[max_process];//will have the array for all the processes
int CPU_HOLDER=-1;//which process is holding the current CPU

//Function Declaration
void handle_arrival_event(int current_time,int pid,int *assign_pid,\
                            int process_times[][2],\
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail);
int schedule_like_FCFS(int process_times[][2]);


int main(){
    //Reading the process times from the txt file
    char filename[100]="processes.csv";
    int process_times[max_process][2];
    read_process_file(process_times,filename);
    printf("Starting the scheduling procedure\n");


    return 0;
}
/*                       FCFS Schedular                      */
Event* create_event(int pid,eventType type,int time){
    Event *eve=(Event *)malloc(sizeof(Event));
    eve->pid=pid;
    eve->type=type;
    eve->time=time;

    return eve;
}
int schedule_like_FCFS(int process_times[][2]){
    //Initializing the ready queue
    Queue *head=NULL,*tail=NULL;
    //Initializing the event heap
    int heap_size=-1;//the fill size of our heap
    Event *event_heap[max_events];
    //Initializing the pid for a new entry
    int assign_pid=0;
    //Initializing the clock (tick-toc)
    int current_time=0;

    //Adding the seed event of first arrival
    Event *eve=create_event(assign_pid,Arrival,process_times[assign_pid][0]);
    add_and_min_heapify(&heap_size,eve,event_heap);
    // assign_pid++;

    //Now starting our handling of events
    while(heap_size>=0){
        eve=pop_and_min_heapify(&heap_size,event_heap);
        current_time=eve->time;//beware while ATAT it can go to reverse (cpucomp->arrival which came before)
        if(eve->type==Arrival){
            handle_arrival_event(current_time,eve->pid,&assign_pid,\
                                    process_times,
                                    &heap_size,event_heap,
                                    &head,&tail);
        }
        else if(eve->type==CPUburstComp){

        }
        for(int i=0;i<=heap_size;i++){
            int pid=event_heap[i]->pid;
            eventType type=event_heap[i]->type;
            int time=event_heap[i]->time;
            printf("%d %d %d\n",pid,type,time);
        }
        //Freeing up the space taken by the this event in heap
        free(eve);
    }
    return 0;
}
Process* make_process_entry_FCFS(int pid,int arrival_time,\
                                    int cpu_burst,pState state){
    //Creating the process struct
    Process* p1=(Process*)malloc(sizeof(Process));
    p1->pid=pid;
    p1->arrival_time=arrival_time;
    p1->cpu_burst=cpu_burst;
    p1->burst_left=cpu_burst;
    p1->state=state;

    return p1;
}
void handle_arrival_event(int current_time,int pid,int *assign_pid,\
                            int process_times[][2],\
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail){
    //Now creating the new process  for this arrival
    pState state=Waiting;
    int arrival_time=process_times[pid][0];
    int cpu_burst=process_times[pid][1];
    if(CPU_HOLDER==-1){
        state=Running;
    }
    Process *pnew=make_process_entry_FCFS(pid,arrival_time,cpu_burst,state);
    //Adding the proces to the process table
    process_table[pid]=pnew;

    //Now running this process if the cpu is free
    if(CPU_HOLDER==-1){
        //Assigning this process to cpu
        CPU_HOLDER=pid;
        //Now adding the completion event of this process
        Event *eve=create_event(pid,\
                                CPUburstComp,\
                                current_time+cpu_burst);
        //Adding this event to the event heap
        add_and_min_heapify(heap_size,eve,event_heap);
    }
    else{
        //if cpu in not avallable add the event to event queue
        *tail=push_to_queue(pid,*tail);
        if(*head==NULL){
            *head=*tail;
        }
    }
    // //Finally incrementing the pid for next process
    // *assign_pid+=1;//no need cuz we will be using the pid in event

    //Now handling the arrival of next process
    arrival_time=process_times[*assign_pid][0];
    cpu_burst=process_times[*assign_pid][1];
    if(current_time>arrival_time){
        //add arrival event if this arrival time is less than the current time
        Event* eve=create_event(*assign_pid,Arrival,arrival_time);
        //adding the event to event heap
        add_and_min_heapify(heap_size,eve,event_heap);
        *assign_pid+=1;
    }
}
