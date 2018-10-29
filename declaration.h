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

/*                  Function Declarations                       */
void add_and_min_heapify(int *size,Event *new_eve,Event *event_heap[]);
Event* pop_and_min_heapify(int *size,Event *event_heap[]);
int read_process_file(int process_times[][2],char filename[]);
void bubble_sort(int size,int process_times[][2]);

/*                  Queue Definition                            */
typedef struct queue{
    int pid;            //holding the process id
    struct queue *next;//hand to next one
}Queue;

Queue* push_to_queue(int pid,Queue *tail);
Queue* pop_from_queue(Queue *head);
