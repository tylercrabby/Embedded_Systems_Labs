#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

struct file_data    // collection of information from input files
{
    int periodic_count;
    int aperiodic_count;
    int sim_time;
    int file_len;
};

struct task_data    // basic task struct
{
    int period;
    int execute;
    string task_name;
};

class Node  // task object, made to build linked lists
{
    public:
    int release_time;
    int period;
    int execute;
    int EDF_deadline;
    int run_time;
    string task_name;
    bool released;
    bool periodic;
    Node *next;
};

int get_max_period(Node* periodic)  // returns the maximum task period from given linked list
{
    int max = 0;    // return variable
    Node* temp = periodic;
    while(temp != NULL)
    {
        if(temp->period > max)  // detect larger period
        {
            max = temp->period;     // update return variable
        }
        temp = temp->next;
    }
    return max;     // return max
}

int get_max_release(Node* aperiodic)    // return the maximum release time from given linked list
{
    int max = 0;    // return variable
    Node* temp = aperiodic;
    while(temp != NULL)
    {
        if(temp->period > max)  // detect larger release time
        {
            max = temp->period;     // update return variable
        }
        temp = temp->next;
    }
    return max;     // return max
}

task_data get_task_data(string input)   // function for extracting task data from input files
{
    task_data new_task;     // return variable
    int counter = 0;
    stringstream ss(input);     // open file
    string word;
    while(ss >> word)   // iterate through parsed string
    {
        if(counter == 0)    // extract task name and eliminate ',' character
        {
            new_task.task_name = word;
            new_task.task_name.erase(remove(new_task.task_name.begin(), new_task.task_name.end(), ','), new_task.task_name.end());
        }
        else if(counter == 1)   // extract execute time
        {
            new_task.execute = stoi(word);
        }
        else    // extract period and reset counter
        {
            new_task.period = stoi(word);
            counter = 0;
        }
        counter++;
    }
    return new_task;    // return new_task
}

void print_file(string I_D)     // test function to read input files: Not used in executable
{
    string tp;
    fstream newfile;
    newfile.open(I_D, ios::in);
    while(getline(newfile, tp))
    {
        cout << tp << "\n";
    }
    newfile.close();
}

void printList(Node *node)      // test function to print lists: Not used in executable
{
    while (node != NULL)
    {
        cout<<node->task_name<<"\t\t"<<node->execute<<"\t\t"<<node->period<<"\n";
        node = node->next;
    }
}

file_data get_file_data(string I_D)     // collects inportant info for generating tasks
{
    file_data data;    // generate return variable
    int counter = 0;
    string tp;
    fstream newfile;
    newfile.open(I_D, ios::in);     // open file
    while(getline(newfile, tp))
    {
        if(counter == 0)
        {
            data.periodic_count = stoi(tp);    // ID periodic task count
            //cout << tp << "\n";
        }
        else if(counter == 1)
        {
            data.sim_time = stoi(tp);  // ID sim time
            //cout << tp << "\n";
        }
        else if(counter == (data.periodic_count + 2))
        {
            data.aperiodic_count = stoi(tp);   // ID aperiodic task count
            //cout << tp << "\n";
            break;  // break once data is gathered
        }
        counter++;
    }
    newfile.close();    // close file
    data.file_len = counter;
    return data;    // return data
}

void append(Node** head_ref, int execute, int period, string name)
{
    Node* new_node = new Node();    // generate new node
    Node *last = *head_ref;
    new_node->execute = execute;
    new_node->period = period;
    new_node->task_name = name;     // attach to end of list
    new_node->next = NULL;
    if (*head_ref == NULL)      // handles empty list
    {
        *head_ref = new_node;
        return;
    }
    while(last->next != NULL)
    {
        last = last->next;
    }
    last->next = new_node;
}

void swap_data(Node* first, Node* second)   // swaps data in two nodes
{
    Node* temp = new Node;  // create temporary variable

    temp->EDF_deadline = first->EDF_deadline;   // temp holds onto first data
    temp->execute = first->execute;
    temp->period = first->period;
    temp->released = first->released;
    temp->run_time = first->run_time;
    temp->task_name = first->task_name;
    temp->release_time = first->release_time;
    temp->periodic = first->periodic;

    first->EDF_deadline = second->EDF_deadline;     // first hold onto second data
    first->execute = second->execute;
    first->period = second->period;
    first->released = second->released;
    first->run_time = second->run_time;
    first->task_name = second->task_name;
    first->release_time = second->release_time;
    first->periodic = second->periodic;

    second->EDF_deadline = temp->EDF_deadline;      // second holds onto temp data
    second->execute = temp->execute;
    second->period = temp->period;
    second->released = temp->released;
    second->run_time = temp->run_time;
    second->task_name = temp->task_name;
    second->release_time = temp->release_time;
    second->periodic = temp->periodic;
}

//############################################################################################## RMA Stuff

void set_RM_priority(Node* head)        // bubble sort for the win
{
    Node* temp = head;
    Node* next = temp->next;
    bool sort = true;
    while(sort)
    {
        temp = head;    // start at head of list
        next = temp->next;
        sort = false;       // reset flag
        while(next != NULL)
        {
            if(next->period < temp->period)
            {
                sort = true;            // flag that changes were made
                swap_data(next, temp);
            }
            temp = temp->next;
            next = next->next;      // iterate through list
        }
    }
    temp = head;
    while(temp != NULL)
    {
        temp->released = false;     // clear release variables
        temp = temp->next;
    }
}

int preempt(Node* curr, ofstream& out, int time)     // function for running aperiodic tasks
{
    out << "\t\t\t" << curr->task_name << " preempts at time: " << time << "\n";
    curr->run_time = curr->execute;
    while(true)
    {
        out << "\t\t\t\t" << curr->task_name << " running for " << curr->run_time << "\n";
        curr->run_time--;
        if(curr->run_time == 0)
        {
            break;
        }
    }
    //out << "\t\t\t" << curr->task_name << " completed at time: " << time+curr->execute << "\n";
    //cout << time << "\t" << curr->execute << "\n";
    return curr->execute + time;
}

int RMA_releases(Node* head, int time, ofstream& out)   // releases tasks
{
    int time_return = time;
    Node* curr = head;
    while(curr != NULL)
    {
        if(time % curr->period == 0 && curr->periodic)  // detect releases from periodic tasks
        {
            if(curr->run_time > 0)  // detect missed deadline, rerelease
            {
                out << "WARNING: " << curr->task_name << " missed deadline. Rereleasing for next period time: " << time << "\n";
                curr->run_time = curr->execute;
                curr->released = true;
            }else   // release task
            {
                out << curr->task_name << " released at time: " << time << "\n";
                curr->released = true;
            }
        }else if(time == curr->release_time && !curr->periodic)     // detect releases from aperiodic tasks
        {
            time_return = preempt(curr, out, time);
            //cout << "\t" << time_return << "\n";
            //out << curr->task_name << " released at time: " << time << "\n";
            //curr->released = true;
        }
        curr = curr->next;  // iterate through list
    }
    //cout << "\t" << time_return << "\n";
    return time_return;
}

Node* get_RMA(Node* head, Node* curr)   // find next task for RMA
{
    Node* temp = head;  // start at task head
    Node* next;     // return variable
    while(temp != NULL)
    {
        if(temp->released && temp != curr)  // find first released task and break
        {
            next = temp;
            break;
        }
        temp = temp->next;  // iterate through list
    }
    return next;    // return next
}

void RM_sim(Node* periodic, Node* aperiodic, string file_name, int sim_time)    // simulation function RM task 3
{
    Node* curr = periodic;  // linked list for periodic tasks
    //set_aperiodic(aperiodic);
    while(curr != NULL)
    {
        curr->periodic = true;      // initialize list as periodic
        if(curr->next == nullptr)
        {
            //curr->next = aperiodic;
            break;
        }
        curr = curr->next;  // iterate through list
    }
    set_RM_priority(periodic);  // organize list based on RM priority
    //printList(periodic);
    Node* next = get_RMA(periodic, curr);   // find first task to run
    file_name = file_name.substr(0, file_name.size()-4);    // generate output file
    file_name = file_name+"_RM.txt";
    ofstream out(file_name);
    out << file_name << "\n";   // do all the fancy stuff here
    int i;
    for(i = 0; i < sim_time; i++)   // loop for running simulation times
    {
        next = get_RMA(periodic, curr);     // get next task to run
        if(next == NULL)    // DONT CHANGE, fixed segmentation faults for some reason
        {
            next = periodic;
        }
        if(curr == NULL && next->released)  // assign current task to next if current is not active
        {
            curr = next;
            out << "\t" << curr->task_name << " started at time: " << i << "\n";    // start task
            curr->run_time = curr->execute;
            next = get_RMA(periodic, next);
        }
        if(curr != NULL && curr->run_time > 0)  // run task for run_time
        {
            out << "\t\t" << curr->task_name << " running for " << curr->run_time << "\n";
            curr->run_time--;
        }else if(curr != NULL && curr->run_time == 0)   // detect completion of a task
        {
            out << "\t" << curr->task_name << " finished at time: " << i << "\n";
            curr->released = false;
            curr = NULL;    // set current to inactive
        }
    }
    out.close();    // close output file
}

void set_aperiodic(Node* head, ofstream& out)   // updates aperiodic tasks
{
    Node* temp = head;
    while(temp != NULL)
    {
        temp->release_time = temp->period;  // change release time to match 'period' from input file
        temp->period = 1;       // set aperiodic tasks to have highest possible priority
        temp->periodic = false;     // set to aperiodic
        temp = temp->next;      // iterate through list
    }
}

void RM_sim_aperiodic(Node* periodic, Node* aperiodic, string file_name, int sim_time)  // simulation function RM task 4
{
    file_name = file_name.substr(0, file_name.size()-4);    // generate output file
    file_name = file_name+"_RM.txt";
    ofstream out(file_name);
    out << file_name << "\n";
    Node* curr = periodic;          // point to periodic
    set_aperiodic(aperiodic, out);
    while(curr != NULL)
    {
        curr->periodic = true;
        if(curr->next == nullptr)       // detect end of list
        {
            curr->next = aperiodic;     // attach aperiodic list to end of periodic list
            break;
        }
        curr = curr->next;  // iterate through list
    }
    set_RM_priority(periodic);      // update list priorities
    //printList(periodic);
    Node* next = get_RMA(periodic, curr);   // get first task to run
    int i;
    int curr_time;
    for(i = 0; i < sim_time; i++)       // loop to run simulation
    {
        curr_time = i;
        i = RMA_releases(periodic, i, out);     // update releases based on current time 'i' and preempt aperiodic
        if(curr_time != i)
        {
            curr_time++;
        }
        while(curr_time != i)                   // loop through missed time while aperiodic was active
        {
            RMA_releases(periodic, curr_time, out);
            curr_time++;
        }
        RMA_releases(periodic, i, out);     // update task releases
        next = get_RMA(periodic, curr);     // find next task to run
        if(next == NULL)                    // DO NOT CHANGE: fixes segmentation faults for some reason
        {
            next = periodic;
        }
        if(curr == NULL && next->released)  // assign next task if current is inactive
        {
            curr = next;
            out << "\t" << curr->task_name << " started at time: " << i << "\n";
            curr->run_time = curr->execute;
            next = get_RMA(periodic, next);
        }
        if(curr != NULL && curr->run_time > 0)  // run task for run_time
        {
            out << "\t\t" << curr->task_name << " running for " << curr->run_time << "\n";
            curr->run_time--;
        }else if(curr != NULL && curr->run_time == 0)   // detect completion of task
        {
            out << "\t" << curr->task_name << " finished at time: " << i << "\n";
            curr->released = false;
            curr = NULL;
        }
    }
    //printList(periodic);
    out.close();    // close output file
}

//################################################################################################# EDF Stuff

void update_EDF_priority(Node* head, int time)  // bubble sort round two
{
    Node* temp = head;      // start at list head
    Node* next = temp->next;
    bool sort = true;
    while(sort)
    {
        sort = false;           // reset flag
        while(next != nullptr)
        {
            if(next->EDF_deadline < temp->EDF_deadline)
            {
                sort = true;            // set flag
                swap_data(next, temp);  // swap data in nodes
            }
            temp = temp->next;          // iterate through list
            next = next->next;
        }
    }
}

Node* get_EDF(Node* head, Node* curr)   // returns the highest priority task EDF
{
    Node* temp = head;      // start at list head
    Node* next;
    while(temp != NULL)
    {
        if(temp->released && temp != curr)      // find first available task and break
        {
            next = temp;
            break;
        }
        temp = temp->next;      // iterate through list
    }
    return next;    // return next
}

int EDF_releases(Node* head, int time, ofstream& out)
{
    int time_return = time;
    Node* curr = head;
    while(curr != NULL)
    {
        if(time % curr->period == 0 && curr->periodic)      // detect releases from periodic tasks
        {
            if(curr->run_time > 0 && time != 0)     // detect missed deadline, rerelease
            {
                out << "WARNING: " << curr->task_name << " missed deadline. Rereleasing for next period time: " << time << "\n";
                curr->run_time = curr->execute;
                curr->released = true;
            }else                                   // detect successful task completion
            {
                out << curr->task_name << " released at time: " << time << "\n";
                curr->released = true;
                curr->EDF_deadline = curr->EDF_deadline + curr->period;
            }
        }else if(time == curr->release_time && !curr->periodic)     // detect release from aperiodic tasks
        {
            time_return = preempt(curr, out, time);
        }
        curr = curr->next;      // iterate through list
    }
    return time_return;
}

void EDF_sim(Node* periodic, Node* aperiodic, string file_name, int sim_time)   // sim function EDF Task 3
{
    //cout << "starting edf sim\n";
    Node* curr = periodic;      // point to list head
    Node* next;
    while(curr != NULL)
    {
        //cout << curr->task_name << "\n";
        curr->EDF_deadline = curr->period;  // update deadlines
        curr->released = false;
        if(curr->periodic)
        {
            curr->release_time = 0;     // set initial release times to 0
        }
        curr = curr->next;  // iterate through list
    }
    file_name = file_name.substr(0, file_name.size()-4);    // generate output file
    file_name = file_name+"_EDF.txt";
    ofstream out(file_name);
    out << file_name << "\n";
    curr = NULL;
    int i;
    int curr_time;
    for(i = 0; i < sim_time; i++)   // loop for running simulation
    {
        //cout << i << "\n";
        curr_time = i;
        i = EDF_releases(periodic, i, out);     // update releases based on current time 'i' and preempt aperiodic
        if(curr_time != i)
        {
            curr_time++;
        }
        while(curr_time != i)                   // loop through missed time while aperiodic was active
        {
            RMA_releases(periodic, curr_time, out);
            curr_time++;
        }
        update_EDF_priority(periodic, i);   // update EDF priorities
        EDF_releases(periodic, i, out);     // release tasks according to time 'i'
        next = get_EDF(periodic, curr);     // find next task to run
        if(next == NULL)                    // DO NOT CHANGE: removes segmentation faults
        {
            next = periodic;
        }
        if(curr == NULL && next->released)  // assigns current if current is inactive
        {
            curr = next;
            out << "\t" << curr->task_name << " started at time: " << i << "\n";
            curr->run_time = curr->execute;
            //next = get_EDF(periodic, next);
        }
        if(curr != NULL && curr->run_time > 0)  // run task for run_time
        {
            out << "\t\t" << curr->task_name << " running for " << curr->run_time << "\n";
            curr->run_time--;
        }else if(curr != NULL && curr->run_time == 0)   // detect successful completion of task
        {
            out << "\t" << curr->task_name << " finished at time: " << i << "\n";
            curr->released = false;
            curr = NULL;
        }
    }
    out.close();
}

void EDF_sim_aperiodic(Node* periodic, Node* aperiodic, string file_name, int sim_time)     // sim function EDF Task 4
{
    file_name = file_name.substr(0, file_name.size()-4);    // generate output file
    file_name = file_name+"_EDF.txt";
    ofstream out(file_name);
    out << file_name << "\n";
    Node* curr = periodic;  // point to list head, aperiodic tasks already attached from RM simulation
    Node* next;
    while(curr != NULL)     // initialize tasks
    {
        curr->EDF_deadline = curr->period;
        curr->released = false;
        curr->periodic = true;
        if(curr->periodic)
        {
            curr->release_time = 0; // release periodic tasks at time 0
        }
        curr = curr->next;      // iterate through list
    }
    curr = NULL;
    for(int i = 0; i < sim_time; i++)   // loop for simulation
    {
        update_EDF_priority(periodic, i);   // update EDF priority list
        EDF_releases(periodic, i, out);     // release tasks according to time 'i'
        next = get_EDF(periodic, curr);     // find next task to run
        if(next == NULL)                    // DO NOT CHANGE: fixes segmentation faults
        {
            next = periodic;
        }
        if(curr == NULL && next->released)  // assigns current if current is inactive
        {
            curr = next;
            out << "\t" << curr->task_name << " started at time: " << i << "\n";
            curr->run_time = curr->execute;
            //next = get_EDF(periodic, next);
        }
        if(curr != NULL && curr->run_time > 0)  // runs task for run_time
        {
            out << "\t\t" << curr->task_name << " running for " << curr->run_time << "\n";
            curr->run_time--;
        }else if(curr != NULL && curr->run_time == 0)   // detects completion of task
        {
            out << "\t" << curr->task_name << " finished at time: " << i << "\n";
            curr->released = false;
            curr = NULL;
        }
    }
    out.close();
}