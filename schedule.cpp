#include <bits/stdc++.h>
using namespace std;

//Structure definition
struct Process{
    //Process IDs
    int pid;

    //process info
    int arrival_time;
    int cpu_burst;
    string sched_policy;

    //process state info
    char state;
    int time_quanta;
    bool preemption;
};
//Structure for events
enum eventType{Arrival,CPUburstComp,TimerExpired};
struct Event{
    int pid;//the id which generated this event
    eventType type;
    double time;//time of occurance of this event
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
    if(p1.first==p2.first){
        return p1.second<=p2.second;
    }
    else if(p1.first<p2.first){
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
struct Event create_event(int pid,eventType type,double time){
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

int schedule_like_FCFS(vector<pair<int,int>> process_times){
    //Initializing the event heap (min heap based on the event time)
    vector<struct Event> event_pq;
    //Making this vector as a heap
    make_heap(event_pq.begin(),event_pq.end(),event_comparater);

    //Adding the first event which will be due to arrival of a process
    int assign_pid=0;
    struct Event e1=create_event(assign_pid,Arrival,process_times[assign_pid].second);
    push_heapify(e1,event_pq);
    cout<<event_pq.front().time<<endl;
    return 0;
}
