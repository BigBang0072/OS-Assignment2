#include <stdio.h>
#include <stdlib.h>
#include "declaration.h"

//Global Variables
#define max_process 50
Process process_table[max_process];//will have the array for all the processes
int CPU_HOLDER=-1;//which process is holding the current CPU

int main(){
    //Reading the process times from the txt file
    char filename[100]="processes.csv";
    int process_times[max_process][2];
    read_process_file(process_times,filename);
    return 0;
}
