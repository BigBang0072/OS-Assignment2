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
        int min=event_heap[idx]->time;
        int mincidx=-1;
        if(cidx1<=*size && min>event_heap[cidx1]->time){
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
        printf("pid:%d A:%d C:%d\n",k,process_times[k][0],process_times[k][1]);
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

//Adding the priority queue for the multilevel queue
//Queue management functions
void push_to_Mqueue(Process *proc,int *rrSize,Process *rrQueue[],\
                                int *fcSize,Process *fcQueue[]){
    //Adding the process to the appropriate queue
    if(proc->cpu_burst<=8){
        printf("Adding the process:%d to RR Queue\n",proc->pid);
        //Specifying the scheduling policy
        proc->sched_policy='R';//round-robin
        add_and_maxprio_heapify_queue(rrSize,proc,rrQueue);
    }
    else{
        printf("Adding the process:%d to FCFS Queue\n",proc->pid);
        //Specifying the scheduling policy
        proc->sched_policy='F';//first come cirst serve
        add_and_maxprio_heapify_queue(fcSize,proc,fcQueue);
    }
}

Process* pop_from_Mqueue(int *rrSize,Process *rrQueue[],
                        int *fcSize,Process *fcQueue[]){
    /*
    The round robin queue has the higher priority than
    the first come first serve queue.
    */
    if(*rrSize!=-1){
        return pop_and_maxprio_heapify_queue(rrSize,rrQueue);
    }
    else{
        //will return NULL if the heap is empty. handle appropriately
        return pop_and_maxprio_heapify_queue(fcSize,fcQueue);
    }
}
//Heapify related function
void add_and_maxprio_heapify_queue(int *size,Process *newP,Process *ready_heap[]){
    /*
        We are going with the two heap approach for the
        implementation of multilevel queue.
        So, check for priority among the two queues
        while using.
    */
    //Incrementing the size of the heap
    *size+=1;
    //Appending the new process to the end
    ready_heap[*size]=newP;

    //Now its time to heapify the whole heap
    int idx=*size;
    while(idx>=1){
        //Getting the parent index
        int pidx=(idx-1)/2;
        //Retreiving the process pointer for ease
        Process *child=ready_heap[idx];
        Process *parent=ready_heap[pidx];
        int swap_flag=0;

        //whoever has arrived forst will stay up in heap
        if(child->num_burst_taken < parent->num_burst_taken){
            swap_flag=1;
        }
        else if(child->num_burst_taken==parent->num_burst_taken && \
                child->arrival_time < parent->arrival_time){
            swap_flag=1;
        }
        else if(child->num_burst_taken==parent->num_burst_taken &&\
                child->arrival_time == parent->arrival_time &&\
                child->pid < parent->pid){
            swap_flag=1;
        }

        if(swap_flag==1){
            //Swapping the place
            ready_heap[idx]=parent;
            ready_heap[pidx]=child;
            idx=pidx;
        }
        else{
            break;
        }
    }
}

Process* pop_and_maxprio_heapify_queue(int *size,Process* ready_heap[]){
    //Handling the base case
    if(*size==-1){
        return NULL;
    }

    //Now takin out the maximum priority case
    Process *ret_proc=ready_heap[0];
    //Now bringing the last element to front
    ready_heap[0]=ready_heap[*size];
    //Decrementing the size of heap
    *size-=1;

    //Now we have to heapify (ensure that only the max prio is at top)
    int idx=0;
    while(idx<=*size){
        //Getting the children index
        int cidx1=2*idx+1;
        int cidx2=2*idx+2;

        int maxcidx=-1;
        int minvalB=ready_heap[idx]->num_burst_taken;
        int minvalT=ready_heap[idx]->arrival_time;

        //Now checking if the swap is needed or not
        if(cidx1<=*size && ready_heap[cidx1]->num_burst_taken < minvalB){
            maxcidx=cidx1;
            minvalB=ready_heap[cidx1]->num_burst_taken;
            minvalT=ready_heap[cidx1]->arrival_time;
        }
        else if(cidx1<=*size && ready_heap[cidx1]->num_burst_taken==minvalB &&\
                ready_heap[cidx1]->arrival_time < minvalT){
            //if the burst taken is same but the child arrivaed earlier
            maxcidx=cidx1;
            minvalT=ready_heap[cidx1]->arrival_time;
        }

        //Checking the second children
        if(cidx2<=*size && ready_heap[cidx2]->num_burst_taken < minvalB){
            maxcidx=cidx2;
            // minvalB=ready_heap[cidx2]->num_burst_taken;
            // minvalT=ready_heap[cidx2]->arrival_time;
        }
        else if(cidx2<=*size && ready_heap[cidx2]->num_burst_taken==minvalB &&\
                ready_heap[cidx2]->arrival_time < minvalT){
            maxcidx=cidx2;
        }

        //if the children are equi-likely
        if(cidx1<=*size && cidx2<=*size &&\
            ready_heap[cidx1]->num_burst_taken==ready_heap[cidx2]->num_burst_taken &&\
            ready_heap[cidx1]->arrival_time==ready_heap[cidx2]->arrival_time){

            //Now choosing the the process with lower pid
            if(ready_heap[cidx1]->pid<ready_heap[cidx2]->pid){
                maxcidx=cidx1;
            }
            else{
                maxcidx=cidx2;
            }
        }

        //Now making the swap if applicable
        if(maxcidx!=-1){
            Process* temp=ready_heap[idx];
            ready_heap[idx]=ready_heap[maxcidx];
            ready_heap[maxcidx]=temp;

            //redoing the problem for the children now
            idx=maxcidx;
        }
        else{
            break;
        }
    }

    //Retruning the maximum priority process
    return ret_proc;
}
