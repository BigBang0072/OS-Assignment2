//Structure definition
typedef enum pState{Running,Waiting,Terminated} pState;
typedef struct Process{
    //Process IDs
    int pid;

    //process info
    int arrival_time;
    int cpu_burst;
    char sched_policy;

    //process state info
    pState state;
    int burst_left;
    int time_quanta;
    int preemption;
}Process;


//Structure for events
typedef enum eventType{Arrival,CPUburstComp,TimerExpired} eventType;
typedef struct Event{
    int pid;//the id which generated this event
    eventType type;
    int time;//time of occurance of this event
}Event;

void add_and_min_heapify(int *size,Event *new_eve,Event *event_heap[]);
Event* pop_and_min_heapify(int *size,Event *event_heap[]);
