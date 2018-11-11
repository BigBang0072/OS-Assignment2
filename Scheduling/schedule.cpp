#include <bits/stdc++.h>
using namespace std;

//Structure definition
enum pState{Running,Waiting,Terminated};
struct Process{
    //Process IDs
    int pid;

    //process info
    int arrival_time;
    int cpu_burst;
    string sched_policy;

    //process state info
    pState state;
    int burst_left;
    int time_quanta;
    bool preemption;
};
//Structure for events
enum eventType{Arrival,CPUburstComp,TimerExpired};
struct Event{
    int pid;//the id which generated this event
    eventType type;
    int time;//time of occurance of this event
};

//Global Variables
vector<struct Process> process_table;//will have the array for all the processes
int CPU_HOLDER=-1;//which process is holding the current CPU


//Function Definitions
vector<pair<int,int>> read_process_file(string filename);
int schedule_like_FCFS(vector<pair<int,int>> process_times);

int main(){
    //Reading the file first
    string filename="processes.csv";
    vector<pair<int,int>> process_times;
    process_times=read_process_file(filename);
    //Printing the vector
    for(unsigned int i=0;i<process_times.size();i++){
        cout<<process_times[i].first<<" "<<process_times[i].second<<endl;
    }
    cout<<"Starting the scheduling process"<<endl;

    //Starting the scheduling process
    schedule_like_FCFS(process_times);

    return 0;
}



/*                      FILE HANDLING                      */
//Building the comparater for our vector
bool comparater(pair<int,int> p1,pair<int,int>p2){
    //sorting in reverse order of their arrival time
    if(p1.first==p2.first){
        return p1.second>=p2.second;
    }
    else if(p1.first>p2.first){
        return true;
    }
    return false;
}
//File reading function
vector<pair<int,int>> read_process_file(string filename){
    /*
    This function will read the file and give a vector of
    arrival time and the cpu burst for ach process int the sorted
    order
    */
    vector<pair<int,int>> process_info;
    //Initializing the regex for extraction
    regex break_pattern("(.*),(.*)");
    smatch smatcher;

    //Starting the filestream
    ifstream process_file(filename);
    string line;
    //Starting to read the file
    if(process_file.is_open()){
        cout<<"Reading the process file"<<endl;
        while(getline(process_file,line)){
            //Extracting the time from the line
            regex_search(line,smatcher,break_pattern);

            //Conveting to int
            int arrival_time=stoi((string)smatcher[1]);
            int cpu_burst=stoi((string)smatcher[2]);
            //cout<<"arrival_time: "<<arrival_time<<" cpu_burst: "<<cpu_burst<<endl;

            //Adding the process info to vector
            process_info.push_back(make_pair(arrival_time,cpu_burst));
        }
        //Closing the reading process
        process_file.close();
    }
    else{
        cout<<"File Reading Failed\n";
    }
    //Sorting the process_info table
    sort(process_info.begin(),process_info.end(),comparater);
    return process_info;
}




/*          First Come FIrst Serve Scheduling Algorithm     */
bool event_comparater(struct Event e1,struct Event e2){
    if(e1.time>e2.time){
        return true;
    }
    return false;
}
//Making the event struct
struct Event create_event(int pid,eventType type,int time){
    struct Event e1;
    e1.pid=pid;
    e1.type=type;
    e1.time=time;

    return e1;
}
//making a one shot function for the insertion and heapifying
void push_heapify(struct Event eve,vector<struct Event> &event_pq){
    //Adding the event to end of vector
    event_pq.push_back(eve);
    //Heapifying the vector
    push_heap(event_pq.begin(),event_pq.end(),event_comparater);
}
//making a one shot function for popping and heapifying
struct Event pop_heapify(vector<struct Event> &event_pq){
    //Pusing the top of heap to end and maintinag the property of heap
    pop_heap(event_pq.begin(),event_pq.end(),event_comparater);
    //Popping the last element
    struct Event eve=event_pq.back();
    event_pq.pop_back();

    return eve;
}

int schedule_like_FCFS(vector<pair<int,int>> &process_times){
    //Initializing the ready queue for the process
    queue<int> ready_queue;

    //Initializing the event heap (min heap based on the event time)
    vector<struct Event> event_pq;
    //Making this vector as a heap
    make_heap(event_pq.begin(),event_pq.end(),event_comparater);

    //Adding the first event which will be due to arrival of a process
    int assign_pid=0;
    struct Event e1=create_event(assign_pid,Arrival,process_times[assign_pid].second);
    push_heapify(e1,event_pq);
    assign_pid++;

    //Initlaizing the Simluation variables
    int current_time=0;//starting the clock (tic-toc)

    //Now starting the SIMLUATION LOOP
    while (event_pq.size()!=0){
        //Getting the most imminent event
        e1=pop_heapify(event_pq);
        //Synchronizing the clock with this event
        current_time=e1.time;//correct

        //Handling the events according to its type
        if(e1.type==Arrival){

        }
        else if(){

        }
    }
    return 0;
}

struct Process make_process_entry_FCFS(int pid,int arrival_time,\
                                    int cpu_burst,pType state){
    //Creating the process struct
    struct Process p1;
    p1.pid=pid;
    p1.arrival_time=arrival_time;
    p1.cpu_burst=cpu_burst;
    p1.burst_left=cpu_burst;
    p1.state=state;

    return p1;
}

void handle_arrival_event(int current_time,int *assign_pid,\
                            vector<pair<int,int>> process_times,\
                            vector<struct Event> &event_pq,\
                            queue<int> &ready_queue){
    /*
    This function will handle the events based on this type
    and also update the current time accordingly.
    */
    //Creatign the current process
    pState state=Waiting;
    int arrival=process_time.back().first;
    int cpu_burst=process_times.back().second;
    //Removing this process from the vector as its handled
    process_times.pop_back();
    if(CPU==-1){
        //Assign the CPU this pid
        CPU=*assign_pid;
        //Since cpu is free we run this process
        state=Running;
        //This will now have to end sometime, thus a future event of completion
        struct Event e1=create_event(*assign_pid,\
                                    CPUburstComp,\
                                    current_time+cpu_burst);
    }
    else{
        //otherwise just adding the process to the ready queue
        ready_queue.push(*assign_pid);
    }
    //Creating the process
    struct Process p1=make_process_entry_FCFS(*assign_pid,arrival,cpu_burst,state);
    //Pushing the process to the Process Table
    process_table.push_back(p1);//it's now indexed with pid
    //Updating the pid for next process
    (*assign_pid)++;

    //Adding the arrival event of next process
    arrival=process_time.back().first;
    cpu_burst=process_time.back().second;
    if(current_time>arrival){
        //Creating the arrival event for next process
        struct Event
    }
}
