#include <bits/stdc++.h>
using namespace std;

//Function Definitions
bool comparater(pair<int,int> p1,pair<int,int>p2);
vector<pair<int,int>> read_process_file(string filename);

int main(){
    //Reading the file first
    string filename="processes.csv";
    vector<pair<int,int>> process_info;
    process_info=read_process_file(filename);
    //Printing the vector
    // for(unsigned int i=0;i<process_info.size();i++){
    //     cout<<process_info[i].first<<" "<<process_info[i].second<<endl;
    // }
    


    return 0;
}

/*                      FILE HANDLING                      */
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

/*          First Come FIrst Serve Scheduling Algorithm     */
int schedule_like_FCFS(){

}
