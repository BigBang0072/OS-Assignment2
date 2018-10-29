#include <stdio.h>
#include <stdlib.h>
#include "declaration.h"

//Global Variables
#define max_process 50
#define max_events 1000
Process *process_table[max_process];//will have the array for all the processes
int CPU_HOLDER=-1;//which process is holding the current CPU

//Function Declaration
void handle_burstComp_event(int *atat/*the total turn around time*/,\
                            int current_time,int pid,int *assign_pid,\
                            int psize,int process_times[][2],
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail);
void handle_arrival_event(int current_time,int pid,int *assign_pid,\
                            int psize,int process_times[][2],\
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail);
int schedule_like_FCFS(int psize,int process_times[][2]);


int main(){
    //Reading the process times from the txt file
    char filename[100]="processes.csv";
    int process_times[max_process][2];
    int psize=read_process_file(process_times,filename);


    printf("Starting the scheduling procedure\n");
    schedule_like_FCFS(psize,process_times);

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
int schedule_like_FCFS(int psize,int process_times[][2]){
    //Initializing the ready queue
    Queue *head=NULL,*tail=NULL;
    //Initializing the event heap
    int heap_size=-1;//the fill size of our heap
    Event *event_heap[max_events];
    //Initializing the pid for a new entry
    int assign_pid=0;
    //Initializing the clock (tick-toc)
    int current_time=0;
    //Initializing the atat varaible to hold the sum of turn around time
    int atat=0;

    //Adding the seed event of first arrival
    printf("Adding the first arrival event\n");
    Event *eve=create_event(assign_pid,Arrival,process_times[assign_pid][0]);
    add_and_min_heapify(&heap_size,eve,event_heap);
    assign_pid++;

    //Now starting our handling of events
    printf("\nStarting the Event handling loop\n");
    while(heap_size>=0){
        printf("\n############# NEW EVENT ###################\n");
        eve=pop_and_min_heapify(&heap_size,event_heap);
        current_time=eve->time;//beware while ATAT it can go to reverse (cpucomp->arrival which came before)

        // int pid=eve->pid;
        // eventType type=eve->type;
        // int time=eve->time;
        // printf("%d %d %d\n",pid,type,time);
        if(eve->type==Arrival){
            handle_arrival_event(current_time,eve->pid,&assign_pid,\
                                    psize,process_times,
                                    &heap_size,event_heap,
                                    &head,&tail);
        }
        else if(eve->type==CPUburstComp){
            handle_burstComp_event(&atat,\
                                    current_time,eve->pid,&assign_pid,\
                                    psize,process_times,\
                                    &heap_size,event_heap,\
                                    &head,&tail);
        }
        // for(int i=0;i<=heap_size;i++){
        //
        // }
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
                            int psize,int process_times[][2],\
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail){
    //Now creating the new process  for this arrival
    printf("Handling the arrival of new process: pid: %d\n\n",pid);
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

        //Printing the information about the ongoing CPU process
        printf("Current Ongoing process in CPU:\n");
        printf("pid: %d\narrival_time: %d\n",pid,arrival_time);
        printf("cpu_burst_time: %d\n",cpu_burst);
        printf("state: %d\n\n",state);
    }
    else{
        //if cpu in not avallable add the event to event queue
        printf("CPU Busy: Adding process PID: %d to ready queue\n",pid);
        *tail=push_to_queue(pid,*tail);
        if(*head==NULL){
            *head=*tail;
        }
    }
    // //Finally incrementing the pid for next process
    // *assign_pid+=1;//no need cuz we will be using the pid in event

    //Now handling the arrival of next process (only if arrival left for some)
    if(*assign_pid<psize){
        arrival_time=process_times[*assign_pid][0];
        //cpu_burst=process_times[*assign_pid][1];
        if(current_time>arrival_time){
            printf("Adding new arrival event to event heap, PID: %d\n",*assign_pid);
            //add arrival event if this arrival time is less than the current time
            Event* eve=create_event(*assign_pid,Arrival,arrival_time);
            //adding the event to event heap
            add_and_min_heapify(heap_size,eve,event_heap);
            *assign_pid+=1;
        }
    }
}

void handle_burstComp_event(int *atat/*the total turn around time*/,\
                            int current_time,int pid,int *assign_pid,\
                            int psize,int process_times[][2],
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail){
    //Terminating this completed event
    printf("CPUburstCompletion of PID: %d\n",pid);
    process_table[pid]->state=Terminated;
    //Free up the CPU_HOLDER
    CPU_HOLDER=-1;
    //Adding the time spend overall (waiting+execution)time by this process
    *atat+=(current_time-process_table[pid]->arrival_time);

    //Now checking the ready queue for any other available processes
    if(*head!=NULL){//ready queue is not empty
        //Since this was first come first serve the process in front has highest priority
        int ready_pid=(*head)->pid;
        *head=pop_from_queue(*head);

        //Now running this process on the CPU
        CPU_HOLDER=ready_pid;
        process_table[ready_pid]->state=Running;

        //Making the cpuburst completion event for this process
        Event *eve=create_event(ready_pid,
                                CPUburstComp,
                                current_time+process_table[ready_pid]->cpu_burst);
        //Adding the event to event queue
        add_and_min_heapify(heap_size,eve,event_heap);

        //Printing the information of current process
        int arrival_time=process_table[ready_pid]->arrival_time;
        int cpu_burst=process_table[ready_pid]->cpu_burst;
        printf("Current Ongoing process in CPU:\n");
        printf("pid: %d\narrival_time: %d\n",pid,arrival_time);
        printf("cpu_burst_time: %d\n",cpu_burst);
        printf("state: %d\n\n",Running);
    }
    else if(*assign_pid<psize){//since ready queue is empty then we have to handle the arrival event from
        printf("Pushing new arrival event to the EventHeap for PID: %d\n",*assign_pid);
        //future somewhere. so this seems the best spots.
        int arrival_time=process_times[*assign_pid][0];
        //int cpu_burst=process_times[*assign_pid][1];

        //Creating a new arrival event
        Event *eve=create_event(*assign_pid,Arrival,arrival_time);
        //Pushing this new event to event queue
        add_and_min_heapify(heap_size,eve,event_heap);
        *assign_pid+=1;
    }
}
