#include <stdio.h>
#include <stdlib.h>
#include "declaration.h"

//Global Variables
#define max_process 50
#define max_events 1000
const int  time_quanta=4;
Process *process_table[max_process];//will have the array for all the processes
int CPU_HOLDER=-1;//which process is holding the current CPU

//Function Declaration
//First Come first serve queue
void handle_burstComp_event(int *atat/*the total turn around time*/,\
                            int current_time,int pid,int *assign_pid,\
                            int psize,int process_times[][2],
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail);
void handle_arrival_event(int current_time,int pid,int *assign_pid,\
                            int psize,int process_times[][2],\
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail);
void handle_incoming_new_process(int *assign_pid,\
                                    int psize,int process_times[][2],\
                                    int *heap_size,Event *event_heap[]);
float schedule_like_FCFS(int psize,int process_times[][2]);
//Multi-level Schedular queue
float schedule_like_Multilevel(int psize,int process_times[][2]);
void handle_arrival_event_MUL(int current_time,int pid,\
                            int process_times[][2],\
                            int *eveSize,Event* event_heap[],\
                            int *rrSize,Process* rrQueue[],\
                            int *fcSize,Process* fcQueue[]);
void handle_timeout_event_MUL(int *atat,int current_time,int pid,\
                                int *eveSize,Event *event_heap[],\
                                int *rrSize,Process *rrQueue[],\
                                int *fcSize,Process *fcQueue[]);
void handle_burstComp_event_MUL(int *atat,int current_time,int pid,\
                                int *eveSize,Event *event_heap[],\
                                int *rrSize,Process *rrQueue[],\
                                int *fcSize,Process *fcQueue[]);

int main(){
    //Reading the process times from the txt file
    char filename[100]="processes.csv";
    int process_times[max_process][2];
    int psize=read_process_file(process_times,filename);


    printf("Starting the FCFS scheduling procedure\n");
    float atat1=schedule_like_FCFS(psize,process_times);

    printf("##########################################################################\n");
    printf("Starting the Multilevel- Queue Procedure\n");
    float atat2=schedule_like_Multilevel(psize,process_times);

    //Printing the results
    printf("\n\n\n\n\n\n\n\n\n\n");
    printf("ATAT1: %0.6f ATAT2: %0.6f\n",atat1,atat2);

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
float schedule_like_FCFS(int psize,int process_times[][2]){
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

        //Taking out the next event to handle
        eve=pop_and_min_heapify(&heap_size,event_heap);
        current_time=eve->time;//beware while ATAT it can go to reverse (cpucomp->arrival which came before)
        printf("Current Sync Time:%d\n",current_time);

        //Handling the events
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
        //Freeing up the space taken by the this event in heap
        free(eve);

        //Now handling the arrival of next process (like a new node in process graph)
        handle_incoming_new_process(&assign_pid,\
                                    psize,process_times,\
                                    &heap_size,event_heap);

    }

    //Printing the atat time
    printf("################# GAME OVER ######################\n");
    float ATAT=(float)atat/psize;
    printf("The ATAT time for this scheduling: %0.6f\n",ATAT);

    return ATAT;
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
}
void handle_burstComp_event(int *atat/*the total turn around time*/,\
                            int current_time,int pid,int *assign_pid,\
                            int psize,int process_times[][2],
                            int *heap_size,Event *event_heap[],\
                            Queue **head,Queue **tail){
    //Terminating this completed event
    printf("CPUburstCompletion of PID: %d\n\n",pid);
    process_table[pid]->state=Terminated;
    //Free up the CPU_HOLDER
    CPU_HOLDER=-1;
    //Adding the time spend overall (waiting+execution)time by this process
    *atat+=(current_time-process_table[pid]->arrival_time);

    //Now checking the ready queue for any other available processes
    if(*head!=NULL){//ready queue is not empty
        printf("Fetching new process from ready Queue\n");
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
        printf("pid: %d\narrival_time: %d\n",ready_pid,arrival_time);
        printf("cpu_burst_time: %d\n",cpu_burst);
        printf("state: %d\n\n",Running);
    }
}
void handle_incoming_new_process(int *assign_pid,\
                                    int psize,int process_times[][2],\
                                    int *heap_size,Event *event_heap[]){
    /*Since this is FCFS the process will end once complete
    //so, the cuurrent time will be updatad once it ends
    //so, all the process which comes between that time need to be put
    in the ready queue so an event generatio is needed*/

    //Checking whether new next process is eligible to enter or not

    if(*assign_pid<psize){
        //Checking the arrival time of new process
        int add_flag=1;
        int arrival_time=process_times[*assign_pid][0];

        if(*heap_size>=0){
            //this is actuall at "could be" present current time
            int next_event_time=event_heap[0]->time;
            if(next_event_time<=arrival_time){
                add_flag=0;
            }
        }

        if(add_flag==1){
            printf("Adding new arrival event to event heap, PID: %d\n",*assign_pid);
            //add arrival event if this arrival time is less than the current time
            Event* eve=create_event(*assign_pid,Arrival,arrival_time);
            //adding the event to event heap
            add_and_min_heapify(heap_size,eve,event_heap);
            *assign_pid+=1;
        }
    }
}

/*                       Multilevel Schedular              */

//Main handler for the scheduling
float schedule_like_Multilevel(int psize,int process_times[][2]){
    /*
    This function will schedule the precess using the
    multilevel queue and calculate the average turnaround time.
    */
    //Initializing the two queues for multilevel handling
    int rrSize=-1,fcSize=-1;//initial size of the heap
    Process *rrQueue[max_process],*fcQueue[max_process];

    //initilaizing the Event heap
    int eveSize=-1;
    Event *event_heap[max_events];

    //Initializing the initial pid,current time and the atat variable
    int assign_pid=0;
    int current_time=0;
    int atat=0;

    //Adding the arrival event for the first process to start Shree Ganesh
    printf("Adding the first arrival event to Event Heap\n");
    Event *eve=create_event(assign_pid,Arrival,process_times[assign_pid][0]);
    add_and_min_heapify(&eveSize,eve,event_heap);
    assign_pid++;

    //Starting the event handling loop
    printf("Starting the event handling loop\n");
    while(eveSize>=0){
        printf("\n############# NEW EVENT ###################\n");

        //Taking out the next event to handle
        eve=pop_and_min_heapify(&eveSize,event_heap);
        current_time=eve->time;
        printf("Current Sync Time:%d\n",current_time);

        if(eve->type==Arrival){
            handle_arrival_event_MUL(current_time,eve->pid,\
                                    process_times,\
                                    &eveSize,event_heap,\
                                    &rrSize,rrQueue,\
                                    &fcSize,fcQueue);
        }
        else if(eve->type==CPUburstComp){
            handle_burstComp_event_MUL(&atat,current_time,eve->pid,\
                                    &eveSize,event_heap,\
                                    &rrSize,rrQueue,\
                                    &fcSize,fcQueue);
        }
        else if(eve->type==TimerExpired){
            handle_timeout_event_MUL(&atat,current_time,eve->pid,\
                                    &eveSize,event_heap,\
                                    &rrSize,rrQueue,\
                                    &fcSize,fcQueue);
        }

        //freeing up the space for this event
        free(eve);

        //Handling the arrival of new process in the meantime
        handle_incoming_new_process(&assign_pid,\
                                    psize,process_times,\
                                    &eveSize,event_heap);

        // for(int i=0;i<=rrSize;i++){
        //     printf("%d\n",rrQueue[i]->pid);
        // }
        printf("\n");
    }

    //Printing the atat time
    printf("################# GAME OVER ######################\n");
    float ATAT=(float)atat/psize;
    printf("The ATAT time for this scheduling: %0.6f\n",ATAT);

    return ATAT;
}
Process *make_process_entry_MUL(int pid,\
                                int arrival_time,\
                                int cpu_burst,\
                                pState state,\
                                int time_quanta){
    //Creating the process entry with elements specialized for
    //he multilevel queue scheduling.

    //Allocating the memory
    Process *proc=(Process *)malloc(sizeof(Process));

    //Filling the process details
    proc->pid=pid;
    proc->arrival_time=arrival_time;
    proc->cpu_burst=cpu_burst;
    proc->burst_left=cpu_burst;
    proc->state=state;
    proc->last_arrival_time=arrival_time;//used while scheduling
    proc->time_quanta=time_quanta;

    return proc;
}
void print_process_info(Process *proc){
    printf("pid: %d\n",proc->pid);
    printf("arrival_time: %d\n",proc->arrival_time);
    printf("cpu_burst: %d\n",proc->cpu_burst);
    printf("burst_left: %d\n",proc->burst_left);
    printf("last_arrival_time: %d\n",proc->last_arrival_time);
    printf("time_quanta: %d\n",proc->time_quanta);
    printf("state: (0:Running,1:Waiting) %d\n",proc->state);
}

void assign_process_to_cpu(int current_time,\
                            int *eveSize,Event *event_heap[],\
                            int *rrSize,Process *rrQueue[],\
                            int *fcSize,Process *fcQueue[]){
    //Assigning a new process to the CPU based on the condition of ready Queue
    //Fetching out a process from the queue
    Process *rproc=pop_from_Mqueue(rrSize,rrQueue,fcSize,fcQueue);
    //Handling the empty queue
    if(rproc==NULL){
        return;//nothing to do
    }
    printf("Assigning a new process to CPU with pid: %d\n",rproc->pid);

    //Adding the process based on the event type
    if (rproc->sched_policy=='R'){//round robin
        //Giving the access of the CPU to the process
        CPU_HOLDER=rproc->pid;

        //Updating the status of the process
        rproc->state=Running;
        rproc->burst_left-=time_quanta;//if 0 or -ve done

        //Creating a time out event for the RR process
        //(handle completion in timeout event)
        Event *eve=create_event(rproc->pid,TimerExpired,\
                                current_time+time_quanta);
        //Adding the event to event queue
        add_and_min_heapify(eveSize,eve,event_heap);

        //Printing the process information
        print_process_info(rproc);
    }
    else if(rproc->sched_policy=='F'){
        //Handling the FCFS process (once started cannot expire)
        //Giving the access to the CPU about process
        CPU_HOLDER=rproc->pid;

        //Updating the status of the process
        rproc->state=Running;
        rproc->burst_left=0;//after this burst

        //Creating the completion event for this process
        Event *eve=create_event(rproc->pid,CPUburstComp,\
                                current_time+rproc->cpu_burst);
        //Adding the event to event queue
        add_and_min_heapify(eveSize,eve,event_heap);

        //printing the process info
        print_process_info(rproc);
    }
}
void handle_arrival_event_MUL(int current_time,int pid,\
                            int process_times[][2],\
                            int *eveSize,Event* event_heap[],\
                            int *rrSize,Process* rrQueue[],\
                            int *fcSize,Process* fcQueue[]){
    //Creating the new process as an welcome to new kid
    printf("Handling the arrival of new process:%d\n",pid);
    pState state=Waiting;//let is be in waiting until we decide the fate later
    //Creating the process varaible
    Process *proc=make_process_entry_MUL(pid,\
                                        process_times[pid][0],\
                                        process_times[pid][1],\
                                        state,\
                                        time_quanta);
    //Adding the process to the process queue
    process_table[pid]=proc;

    //Adding the process to the ready Queue and let the manager decide priority
    push_to_Mqueue(proc,rrSize,rrQueue,fcSize,fcQueue);

    //Now handling the fate of the process (run to to ready queue)
    if(CPU_HOLDER==-1){//Cpu is free
        //Assigning a process to the CPU
        assign_process_to_cpu(current_time,eveSize,event_heap,\
                                rrSize,rrQueue,fcSize,fcQueue);
    }
    else{
        printf("CPU Busy adding the process to the Queue\n");
    }
}
void handle_timeout_event_MUL(int *atat,int current_time,int pid,\
                                int *eveSize,Event *event_heap[],\
                                int *rrSize,Process *rrQueue[],\
                                int *fcSize,Process *fcQueue[]){
    /*
    This function will handle the timeout event for the round robin
    processes. Since they have to be switched every time quanta.
    */
    //getting the process info-pointer from process table
    Process *proc=process_table[pid];
    //Freeing the status of the CPU
    CPU_HOLDER=-1;

    //Checking if the work completed or still left
    if(proc->burst_left<=0){//work done
        //Terminating the process
        proc->state=Terminated;

        //Adding the turnaround time for this completed process
        *atat+=((current_time+proc->burst_left)-proc->arrival_time);
    }
    else{//not over yet (still left to be done)
        //Changing the state of the process
        proc->state=Waiting;
        proc->last_arrival_time=current_time;//it is arriving again to queue
        //Pushing the process to the queue (for their next turn)
        push_to_Mqueue(proc,rrSize,rrQueue,fcSize,fcQueue);
    }
    //Assigning a new process to cpu (if availble now internally)
    printf("Doing the context Switching after time out\n");
    assign_process_to_cpu(current_time,eveSize,event_heap,\
                            rrSize,rrQueue,fcSize,fcQueue);

}
void handle_burstComp_event_MUL(int *atat,int current_time,int pid,\
                                int *eveSize,Event *event_heap[],\
                                int *rrSize,Process *rrQueue[],\
                                int *fcSize,Process *fcQueue[]){
    /*
    This function will handle the burst completion event
    for the FCFS processes since the RR process will be handled automatically
    */
    //Getting the porcess info-pointer from the process table
    Process *proc=process_table[pid];
    //Freeing up the CPU
    CPU_HOLDER=-1;
    proc->state=Terminated;

    //Adding the turnaround time (from arrival to execution time)
    *atat+=(current_time-proc->arrival_time);

    //Now assigning the new process to the CPU
    printf("CPU burst completed for pid:%d\n",pid);
    assign_process_to_cpu(current_time,eveSize,event_heap,\
                            rrSize,rrQueue,fcSize,fcQueue);
}
