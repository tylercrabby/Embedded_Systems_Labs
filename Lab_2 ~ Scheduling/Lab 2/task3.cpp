#include "utility_v2.h"

int main(int argc, char** argv)
{
    Node* periodic;     // head of periodic linked list
    Node* aperiodic;    // head of aperiodic linked list
    string tp;
    int sim_time = 0;
    file_data file_data;    // struct for extracting file data
    task_data task_data;    // struct for extracting task data
    int counter;
    for(int i = 1; i < argc; i++)   // loop through command line arguments
    {
        counter = 0;
        file_data = get_file_data(argv[i]); // open each command line file
        fstream newfile;
        newfile.open(argv[i], ios::in);     // open file
        while(getline(newfile, tp))
        {
            if(counter == 1)
            {
                sim_time = stoi(tp);
            }
            if(counter >= 2 && counter <= 1+file_data.periodic_count)   // specify periodic task range
            {
                task_data = get_task_data(tp);
                append(&periodic, task_data.execute, task_data.period, task_data.task_name);    // create task list periodic
            }
            else if(counter >= 3+file_data.periodic_count)  //specify aperiodic task range
            {
                task_data = get_task_data(tp);
                append(&aperiodic, task_data.execute, task_data.period, task_data.task_name);   // create task list aperiodic
            }
            counter++;
        }
        aperiodic = nullptr;
        RM_sim_aperiodic(periodic, aperiodic, argv[i], sim_time);
        //cout << "RM completed\n";
        EDF_sim(periodic, aperiodic, argv[i], sim_time);  // create EDF schedule
        //cout << "EDF completed\n";
        newfile.close();    // close file before moving on to next
        //cout << "File closed\n";
        periodic = NULL;
        aperiodic = NULL;
    }
    return 0;
}