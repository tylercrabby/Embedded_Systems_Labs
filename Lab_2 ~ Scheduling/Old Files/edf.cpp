#include "edf.h"

/*
	Take the periodic and aperiodic tasks and use Earliest Deadline First scheduling to craft a schedule.
*/
bool earliest_deadline_first(int argc, char *argv[]) {
	FileManager fm(argc, argv);

	// Build the structture of the scheduler.
	int premptionCounter = 0;
	int missedDeadlineCounter = 0;

	stringstream report;

	vector<task> tasks = fm.get_tasks();
	vector<task> aTasks = fm.get_a_tasks();
	
	// Are we doing both periodic and aperiodic tasks?
	if (fm.get_num_a_tasks() > 0) {
		cout << "Got in.";
		combineTasks(tasks, aTasks);
	} else {
		cout << "Didn't get in.";
	}


	printTasks(tasks);

	// Sort all tasks by earliest deadline.
	//quickSort(tasks, 0, tasks.size() - 1);
	bubbleSort(tasks);

	// Iterate and release the tasks. 
	for(int time = 0; time < fm.get_sim_time(); time++){
		// Give us a glimpse into what is happening.
		if (time % 5 == 0){
			cout << "Time is: " << time;
			printTasks(tasks);
		}
		
		// Release all of the tasks at time = 0
		if (time == 0){
			for(int i = 0; i < tasks.size(); i++) {
				tasks[i].task_state = 1;
			}
		}

		int k = -1;		// A flag needed to see if any deadlines were reassigned.

		// Check for tasks that are hitting their deadlines.
		for (int i = 0; i < tasks.size(); i++){

			// If a deadline is hit, scan the table of task data to recall the deadline again.
			if (tasks[i].period == 0){

				// Need to build a new reference table of tasks to find what durations and deadlines should be.
				vector<task> temp_tasks = fm.get_tasks();
				vector<task> temp_aTasks = fm.get_a_tasks();
				combineTasks(temp_tasks, temp_aTasks);

				// Hunt for the task that just completed.
				for(int j = 0; j < tasks.size(); j++) {
					// Scan the whole task table comparing the names of the completed task to find its next deadline.
					//cout << "Completed task is: " << tasks[i].name << ". Looking at task " << temp_tasks[j].name << ".\n";

					if (tasks[i].name == temp_tasks[j].name) {
						k = j;
						break;
					}
				}

				if (k >= 0) {
					tasks[i].period = temp_tasks[k].period;
					tasks[i].exe_time = temp_tasks[k].exe_time;
					tasks[i].task_state = 1;

					//cout << "Task " << tasks[i].name << "'s new deadline is: " << tasks[i].period << ".\n";

				} else {
					cout << "The task name wasn't found!!!";
					return false;
				}
			}
		}

		// Reprioritize the group only if a deadline has been reassigned.
		if (k >= 0) {
			bubbleSort(tasks);
		}

		// Count down the duration of the next uncompleted task.
		for (int i = 0; i < tasks.size(); i++){
			// Was this task completed?
			if (tasks[i].exe_time == 0 && tasks[i].task_state != 4){
				tasks[i].task_state = 4;
				
				cout << "Task " << tasks[i].name << " was completed at time " << time << ".\n\n";
				report << "Task " << tasks[i].name << " was completed at time " << time << ".\n";
			}

			// Has this task already been completed before? Skip it if it has.
			if (tasks[i].task_state != 4){
				tasks[i].task_state = 2;

				tasks[i].exe_time--;
				break;
			}	
		}
	
		// Decrement all task deadlines in the vector.
		for(int i = 0; i < tasks.size(); i++){
			tasks[i].period--;
		}
	}

	// How are we checking for premption? Using the status flags.

	// Build the report. We need to know: when a task was prempted and when a deadline was missed.	
	cout << report.str();

	// fm.output << report.str();

	return true;
}

/*
	Need a quick way to build a consise task list.
*/
void combineTasks(vector<task>& tasks, vector<task>& aTasks){
		tasks.insert(tasks.end(), aTasks.begin(), aTasks.end());
	}

void printTasks(vector<task> tasks){
	string task_state[] = {"unreleased", "released", "running", "prempted", "completed"};

	cout << endl;
	cout << "Task:  Duration:  Deadline:  Status:\n";
	cout << "------------------------------------\n";

	for(int i = 0; i < tasks.size(); i++){
		cout << "  " << tasks[i].name << "       " << tasks[i].exe_time << "         " << tasks[i].period << "     " << task_state[tasks[i].task_state] << endl;
	}

	cout << endl << endl;
}

void printTasks(vector<task> tasks, vector<task> aTasks){
	string task_state[] = {"unreleased", "released", "running", "prempted", "completed"};

	cout << endl;
	cout << "Task:  Duration:  Deadline:  Status:\n";
	cout << "------------------------------------\n";

	for(int i = 0; i < tasks.size(); i++){
		cout << "  " << tasks[i].name << "       " << tasks[i].exe_time << "         " << tasks[i].period << "     " << task_state[tasks[i].task_state] << endl;
	}
	
	for(int i = 0; i < aTasks.size(); i++){
		cout << "  " << aTasks[i].name << "       " << aTasks[i].exe_time << "         " << aTasks[i].period << "     " << task_state[aTasks[i].task_state] << endl;
	}

	cout << endl << endl;
}

/*
	This does an O(nlog(n)) sort to get the deadlines in order of soonest first.
*/
void quickSort(vector<task>& tasks, int left, int right) {
    int index = partition(tasks, left, right);

    if (left < index - 1)
        quickSort(tasks, left, index - 1);

    if (index < right)
        quickSort(tasks, index, right);
}

// Partition the vector.
int partition(vector<task>& tasks, int left, int right) {
    // Pick our pivot in middle of the vector.
    int pivot = (left + right) / 2;

    while (left <= right) {

        // Find any element on the left that should be on right instead.
        while (tasks[left].period < tasks[pivot].period) {
			++left;
		}

        // Find any element on the right that should be on left instead.
        while (tasks[right].period > tasks[pivot].period) {
			--right;
		};

        if (left <= right) {
			// Make sure tasks with the same deadline aren't swapped if another's duration is shorter.
			if (tasks[left].exe_time > tasks[right].exe_time){
            	swap(tasks[left], tasks[right]);
			}

			++left;
            --right;
        }
    }
    return left;
}

/*
	Quicksort isn't working right when the durations get a little long in the tooth.
*/
void bubbleSort(vector<task>& tasks) {
   	int i, j;
   	bool swapped;
	for (i = 0; i < tasks.size() - 1; i++) {
		swapped = false;
		for (j = 0; j < tasks.size() - i - 1; j++) {
			if ((tasks[j].period > tasks[j + 1].period) || ((tasks[j].period == tasks[j + 1].period) && (tasks[j].exe_time > tasks[j + 1].exe_time))){
				swap(tasks[j], tasks[j + 1]);
				swapped = true;
			}
		}
		
		// IF no two elements were swapped by inner loop, then break.
		if (swapped == false)
			break;
   }
}
