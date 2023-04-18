#include "edf.h"

/*
	Take the periodic and aperiodic tasks and use Earliest Deadline First scheduling to craft a schedule.
*/
bool earliest_deadline_first(int argc, char *argv[]) {
	FileManager fm(argc, argv);

	// Build the structture of the scheduler.
	int preemptionCounter = 0;
	int missedDeadlineCounter = 0;

	stringstream report;

	vector<task> tasks = fm.get_tasks();
	vector<task> aTasks = fm.get_a_tasks();
	
	// Are we doing both periodic and aperiodic tasks?
	if (fm.get_num_a_tasks() > 0) {
		combineTasks(tasks, aTasks);
	}

	// A reference copy of the tasks to reset deadlines and execution times with.
	vector<task> tasksReference(tasks);

	//printTasks(tasks);

	// Sort all tasks by earliest deadline.
	bubbleSort(tasks);

	report << endl << "This Earliest Deadline First program will schedule your tasks for you.\n\n";

	// Iterate and release the tasks. 
	for(int time = 0; time < fm.get_sim_time(); time++){
		// Give us a glimpse into what is happening.
		/*if (time % 5 == 0 || time == 1){
			cout << "Time is: " << time;
			printTasks(tasks);
		}*/
		
		// Release all of the periodic tasks at time = 0
		if (time == 0){
			for (int i = 0; i < tasks.size(); i++) {
				if (!tasks[i].is_a){
					tasks[i].task_state = 1;
				}
			}
		}

		int k = -1;		// A flag needed to see if any deadlines were reassigned.

		// Check for tasks that are hitting their deadlines.
		for (int i = 0; i < tasks.size(); i++){

			// Was this task completed?
			if (tasks[i].exe_time == 0 && tasks[i].task_state != 5){
				tasks[i].task_state = 5;
				
				report << "Task " << tasks[i].name << " was completed at time " << time << ".\n";
			}

			// If a deadline is hit, scan the table of task data to recall the deadline again.
			if (tasks[i].period == 0){

				// Was a deadline missed? Does this task still have duration time left?
				if(tasks[i].exe_time > 0 && tasks[i].task_state != 0){
					tasks[i].task_state = 4;
					report << "!!! Task " << tasks[i].name << " missed its deadline at time " << time << "!!!\n";
					missedDeadlineCounter++;
				}

				// Hunt for the task that just completed.
				for(int j = 0; j < tasks.size(); j++) {
					// Scan the whole task table comparing the names of the completed task to find its next deadline.
					if (tasks[i].name == tasksReference[j].name) {
						k = j;
						break;
					}
				}

				// The aperiodic tasks need to be released and have their deadlines set to 500 mSecs when their first deadlines are hit.
				if (k >= 0) {
					// If the task is aperiodic, then it's only deadline is set to 500. This should only ever be done once.
					if (tasks[i].is_a){
						if (tasks[i].task_state != 5){
							tasks[i].period = (500 - time);
						} else {
							tasks[i].period = -1;		// A flag that will never decrement the period again.
						}
					} else {
						tasks[i].period = tasksReference[k].period;			// Periodic tasks get their periods reassigned.
						tasks[i].exe_time = tasksReference[k].exe_time;		// Their execution times are also repaired.
					}

					if (!tasks[i].is_a) {
						tasks[i].task_state = 1;							// Set the task's state to released.
					} else {
						if (tasks[i].task_state != 5){
							tasks[i].task_state = 1;						// Set the task's state to released.
						}
					}

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

		// 	Go through the tasks and see if multiple tasks are running at the same time. The lower priority tasks all become prempted.
		bool anyTasksCompleted = false;

		for(int i = 0; i < tasks.size(); i++){
			if (tasks[i].task_state == 2){
				// If a task with a closer deadline is already running, set any other previously running tasks to preempted.
				if(anyTasksCompleted){
					preemptionCounter++;
					tasks[i].task_state = 3;

					report << "!!! Task " << tasks[i].name << " was preempted at time " << time << "!!!\n";
				}
				anyTasksCompleted = true;
			}
		}

		// Count down the duration of the next uncompleted task.
		for (int i = 0; i < tasks.size(); i++){

			// Is the task released or has this task already been completed before? Skip it if it has.
			if ( 1 <= tasks[i].task_state && tasks[i].task_state <= 3){
				// Has this task been prempted?
				if(tasks[i].task_state == 3){
					report << "!!! Task " << tasks[i].name << " is no longer preempted at time " << time << "!!!\n";
				}

				// Set the task to running if it is not currently.
				if (tasks[i].task_state != 2) {
					tasks[i].task_state = 2;
				}

				// Decrement the running task's execution time.S
				tasks[i].exe_time--;
				break;
			}	
		}
	
		// Decrement all task deadlines in the vector.
		for(int i = 0; i < tasks.size(); i++){
			if(tasks[i].period >= 0){
				tasks[i].period--;
			}
		}
	}

	// Build the report. We need to know: when a task was prempted and when a deadline was missed.
	report << endl << "There were " << preemptionCounter << " preemptions that happened.\n";
	report << "There were " << missedDeadlineCounter << " tasks that missed their deadlines.\n\n";
	
	//cout << report.str();

	// Push the report to the output file.
	fm.output << report.str();

	return true;
}

/*
	Need a quick way to build a consise task list.
*/
void combineTasks(vector<task>& tasks, vector<task>& aTasks){
	tasks.insert(tasks.end(), aTasks.begin(), aTasks.end());
}

void printTasks(vector<task> tasks){
	//						0			  1			  2			 3			 4		   5
	string task_state[] = {"unreleased", "released", "running", "prempted", "missed", "completed"};

	cout << endl;
	cout << "Task:  Duration:  Deadline:  Status:\n";
	cout << "------------------------------------\n";

	for(int i = 0; i < tasks.size(); i++){
		cout << "  " << tasks[i].name << "       " << tasks[i].exe_time << "         " << tasks[i].period << "     " << task_state[tasks[i].task_state] << endl;
	}

	cout << endl << endl;
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
