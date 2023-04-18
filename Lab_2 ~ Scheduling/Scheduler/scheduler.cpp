/*
Carter Nettesheim
Tyler Crabb
Lab 2 ~ Rate Monotonic and Earliest Deadline First Scheduling
29MAR2023
Dr. Phillips' ECE5780 Embedded Real-Time Systems
*/

#include "scheduler.h"

/*
release_tasks checks the queue of tasks to see which of the tasks needs to be processed first.
*/
void release_tasks(queue<task>* q, vector<task> tasks, int time) {
	if (time == 0) {
		for (int i = 0; i < tasks.size(); i++) {
			q -> push(tasks[i]);
			cout << "Task " << tasks[i].name << " was pushed at time " << time << "." << endl;
		}
	}
	else {
		for (int i = 0; i < tasks.size(); i++) {
			if (time % tasks[i].period == 0) {
				q->push(tasks[i]);
				cout << "task pushed: " << time << " " << tasks[i].period << " " << time % tasks[i].period << endl;
			}
		}
	}
}

vector<task> release_a_tasks(queue<task>* q, vector<task> tasks, int time) {
	if (time != 0) {
		for (int i = 0; i < tasks.size(); i++) {
			if (time % tasks[i].period == 0) {
				q->push(tasks[i]);
				tasks.erase(tasks.begin() + i);
				cout << "a task pushed: " << time << " " << tasks[i].period << " " << time % tasks[i].period << endl;	
			}
		}
	}
	return tasks;
}

/*
	Take the periodic and aperiodic tasks and use Earliest Deadline First scheduling to craft a schedule.
*/
bool edf(vector<task> tasks, vector<task> aTasks, int timeframe){
	// Build the structture of the scheduler.
	int time = 0;
	int premptionCounter = 0;
	int missedDeadlineCounter = 0;

	task prioritizedTasks[tasks.size() + aTasks.size()];

	cout << "\nTotal task count:" << tasks.size() + aTasks.size() << "\n\n";

	// Sort all tasks by earliest deadline.
	for(int i = 0; i < tasks.size() + aTasks.size(); i++){



	}

	// Iterate and release the tasks. 

	// How are we checking for premption? Completion flags?

	return true;
}

/*
This big bad scheduling program takes an input file of tasks, their runtimes, and release windows and creates a schedule to run them all by.
The schedules will be written to an output text file that the user will name.
*/
int main(int argc, char *argv[]) {

	// Check to make sure the user gave an input and an output file name to be used.
	if(argc != 3){
		cout << "\n!!!!!!!!!!\n";
		cout << "This program requires a valid input text file, and some name to call the output file. \n";
		cout << "Please try to run the program again.\n\n";

		return 0;
	}

	ifstream input;					// Read in an input file giving run time, count of tasks to schedule, and each task's duration.
	ofstream output;				// Write the scheduling results to a text file named by the user.

	input.open(argv[1]);			// Open a file frpm the user to get our scheduling constraints.
	
	// Did the user actually specify a valid file name?
	if(!input.good()){
		cout << "\n!!!!!!!!!!\n";
		cout << "That filename: " << argv[1] << ", was either not valid or in the working directory.\n";
		cout << "Please try again with a valid filename.\n\n";
		
		return 0;
	}

	output.open(argv[2], ios::out);	// If the input filename was valid, create an output file to write to.

	// Was that file successfully created?
	if(!output.good()){
		cout << "\n!!!!!!!!!!\n";
		cout << "There was a problem creating your " << argv[2] << " file.\n";
		cout << "Please try again with a valid filename.\n\n";
		
		return 0;
	}

	int num_tasks;					// Fetch the number of tasks, their names, their simulation time, and release times (in milliseconds).
	int num_a_tasks;				// Fetch the number of aperiodic tasks, their names, their simulation time, and release times (in milliseconds).

	// How long is the simulation expected to run for in milliseconds? Keep that data here.
//	int time;
	int sim_time;

	string temp_str;				// A temporary spot to save the task data before it gets moved to the task struct.

	vector<string> lines;			// Build an array of strings to store the task data in.
	vector<task> tasks;				// Build an array of task structs to store task data in.
	vector<task> a_tasks;			// Build an array of asynchronous task structs to store the asyncronous task data in.
	
	queue<task> release_q;			// This queue of tasks will allow us to push and pop tasks as we handle them.
	
	task cur_task;					// We need to know how long a task will be running for.
	task prev_task;					// We also need to know the last task that ran to see if a delay is happening now.
	
	int cur_exe_time_left = 0;		// A decrementor to track when the whole operation is out of time without changing sim_time.

	input >> num_tasks;				// How many total syncronous tasks are we working with?
	input >> sim_time;				// How many milliseconds do we have to build a schedule with?
	
	getline(input, temp_str);		// Read the first task of the group to be scheduled. DOESN'T THIS FIRST TASK GET OVERWRITTEN IN THE FOR LOOP BELOW?

	// Loop through the input file to build a queue of periodic tasks to schedule.
	for (int i = 0; i < num_tasks; i++) {
		getline(input, temp_str);
		lines.push_back(temp_str);
	}

	// Convert the task data from strings to tasks objects and queue them up.
	for (int i = 0; i < num_tasks; i++) {
		stringstream ss(lines[i]);
		task temp_t;

		// These tasks start out unreleased.
		temp_t.state = 0;

		char comma;

		ss >> temp_t.name >> comma >> temp_t.exe_time >> comma >> temp_t.period;
		tasks.push_back(temp_t);
	}

	// Just show off which tasks got queued.
	for (int i = 0; i < num_tasks; i++) {
		cout << tasks[i].name << ' ' << tasks[i].exe_time << ' ' << tasks[i].period << ' ' << stateName[tasks[i].state] << ".\n";
	}

	input >> num_a_tasks;			// Get the count of aperiodic tasks to schedule.

	getline(input, temp_str);		// Grab the first aperiodic task.
	lines.clear();					// Empty out the lines vector so we can store the aperiodic data in it.

	// Loop through the input file to build a queue of aperiodic tasks to schedule.
	for (int i = 0; i < num_a_tasks; i++) {
		getline(input, temp_str);
		lines.push_back(temp_str);
	}
	
	// We are done with the input file.
	if(input.eof()) {
		input.close();
	} else {
		cout << "\n!!!!!!!!!!\n";
		cout << "There was something wrong reading to the end of the input file!!\n";

		return 0;
	}

	// Convert the task data from strings to tasks objects and queue them up.
	for (int i = 0; i < num_a_tasks; i++) {
		stringstream ss(lines[i]);
		task temp_t;
		char comma;

		ss >> temp_t.name >> comma >> temp_t.exe_time >> comma >> temp_t.period;
		a_tasks.push_back(temp_t);
	}
	
	// Show off which aperiodic tasks were queued up.
	for (int i = 0; i < num_a_tasks; i++) {
		cout << a_tasks[i].name << ' ' << a_tasks[i].exe_time << ' ' << tasks[i].period << ' ' << stateName[tasks[i].state] << ".\n";
	}

	/* 
	Rate Monotonic scheduling
	Go through every millisecond within the total simulation time and identify which task (if any) are being run in that millisecond.
	
	for (int time = 0; time < sim_time; time++) {

		// Check which tasks to release.
		release_tasks(&release_q, tasks, time);
		a_tasks = release_a_tasks(&release_q, a_tasks, time);
		
		// Execute first task in the release queue.
		if (cur_exe_time_left == 0) {
			prev_task = cur_task;
			if (!release_q.empty()) {
				cur_task = release_q.front();
				release_q.pop();
				cur_exe_time_left = cur_task.exe_time - 1;
			}
			if (cur_task.name == prev_task.name) {
				output << "idle" << endl;
			}
			else {
				output << time << ": task " << cur_task.name << "\ttime remaining " << cur_exe_time_left << endl;
			}
		}
		else {
			cur_exe_time_left--;
			output << time << ": task " << cur_task.name << "\ttime remaining " << cur_exe_time_left << endl;
		}	
	} */

	// Run the Earliest Deadline First scheduling.
	edf(tasks, a_tasks, sim_time);

	output.close();
	return 0;
}