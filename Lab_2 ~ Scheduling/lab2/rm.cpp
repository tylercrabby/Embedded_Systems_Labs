#include "rm.h"

// sets the task priority for the rate monotonic algorithm
vector<task> set_priority(vector<task> tasks) {
	sort(tasks.begin(), tasks.end(), compare_task); 
	return tasks;
}

// runs the rate monotonic scheduling algorithm on a set of data then outputs to an output file
void rate_monotonic(int argc, char *argv[]) {
	// get tasks from the file reader
	FileManager fm(argc, argv);
	vector<task> tasks = fm.get_tasks();
	vector<task> a_tasks = fm.get_a_tasks();

	// set the task priority based on period **tie goes to first in the list**
	tasks = set_priority(tasks);

	// create a scheduling queue
	queue<task> release_q;
	queue<task> release_aq;

	// declare variables
	task cur_task;
	task prev_task;
	task on_hold;
	int cur_exe_time_left = 0;
	int prev_exe_time_left = 0;
	int hold_exe_time_left = 0;
	vector<char> missed_deadlines;
	int preemptions = 0;

	cur_task.name = UNUSED_TASK;
	on_hold.name = UNUSED_TASK;

	// print out output file header
	fm.output << "Running rate monotonic algorithm\n****************************************" << endl;
	fm.output << "Simulation Time: " << fm.get_sim_time() << " ms" << endl << endl;
	fm.output << fm.get_num_tasks() << " Periodic Tasks:" << endl;
	
	for (int i = 0; i < fm.get_num_tasks(); i++) {
		fm.output << tasks[i].name << " " << tasks[i].exe_time << " " << tasks[i].period << endl;
	}
	
	fm.output << endl;
	fm.output << fm.get_num_a_tasks() << " Aperiodic Tasks:" << endl;

	for (int i = 0; i < fm.get_num_a_tasks(); i++) {
		fm.output << a_tasks[i].name << " " << a_tasks[i].exe_time << " " << a_tasks[i].period << endl;
	}

	fm.output << "****************************************\nBegin: logging significant events" << endl;
	fm.output << "****************************************" << endl;

	// run the rate monotonic algorithm
	for (int time = 0; time < fm.get_sim_time(); time++) {
		// check which tasks to release
		fm.output << release_tasks(&release_q, tasks, time);
		fm.output << release_a_tasks(&release_aq, a_tasks, time);


		// run algorithm
		if (cur_exe_time_left == 0) {
			if (cur_task.name != UNUSED_TASK && cur_task.name != prev_task.name) {
				fm.output << "Time:\t" << time << "\t\tTask " << cur_task.name << " complete" << endl;
			}
			prev_task = cur_task;
			// start periodic tasks as highest priority
			if (!release_q.empty()) {
				cur_task = release_q.front();
				release_q.pop();
				cur_exe_time_left = cur_task.exe_time - 1;
				fm.output << "Time:\t" << time << "\t\tTask " << cur_task.name << " start execution";
				fm.output << "\t exe time: " << cur_task.exe_time << endl;
			}
			// resume preempted task if there is slack time
			else if (on_hold.name != UNUSED_TASK) {
				cur_task = on_hold;
				cur_exe_time_left = hold_exe_time_left;
				on_hold.name = UNUSED_TASK;
				fm.output << "Time:\t" << time << "\t\tTask " << cur_task.name << " resume execution";
				fm.output << "\t exe time: " << cur_exe_time_left + 1 << endl;
			}
			// start aperiodic tasks if there is slack time
			else if (!release_aq.empty()) {
				cur_task = release_aq.front();
				release_aq.pop();
				cur_exe_time_left = cur_task.exe_time - 1;
				fm.output << "Time:\t" << time << "\t\tTask " << cur_task.name << " start execution";
				fm.output << "\t exe time: " << cur_task.exe_time << endl;
			}
		}
		// check for preemptions
		else if (!release_q.empty() && cur_task.is_a && on_hold.name == UNUSED_TASK) {
			on_hold = cur_task;
			hold_exe_time_left = cur_exe_time_left - 1;
			
			cur_task = release_q.front();
			release_q.pop();
			cur_exe_time_left = cur_task.exe_time - 1;

			fm.output << "Time:\t" << time << "\t\tTask " << cur_task.name << " preempted task " << on_hold.name << endl;
			fm.output << "Time:\t" << time << "\t\tTask " << cur_task.name << " start execution";
			fm.output << "\t exe time: " << cur_exe_time_left + 1 << endl;
			preemptions++;
		}
		// continue normal execution
		else {
			cur_exe_time_left--;
		}
		// check deadlines
		fm.output << check_deadline(cur_task, release_q, time, &missed_deadlines);
		fm.output << check_a_deadline(release_q, time, &missed_deadlines);
	}
	
	// print footer header
	fm.output << "Time:\t" << fm.get_sim_time() << "\t\tEnd of sim time " << endl;
	fm.output << endl << "****************************************" << endl;
	fm.output << "Summary" << endl;
	fm.output << "****************************************" << endl;
	
	// print missed deadlines in footer
	for (int i = 0; i < tasks.size(); i++) {
		int count = 0;
		for (int j = 0; j < missed_deadlines.size(); j++) {
			if (tasks[i].name == missed_deadlines[j]) {
				count++;
			}
		}
		if (count != 0) {
			fm.output << "Task " << tasks[i].name << " missed " << count << " deadlines" << endl;
		}
	}

	for (int i = 0; i < a_tasks.size(); i++) {
		int count = 0;
		for (int j = 0; j < missed_deadlines.size(); j++) {
			if (a_tasks[i].name == missed_deadlines[j]) {
				count++;
			}
		}
		if (count != 0) {
			fm.output << "Task " << a_tasks[i].name << " missed " << count << " deadlines" << endl;
		}
	}

	// print footer totals
	fm.output << endl << "Total Missed Deadlines: " << missed_deadlines.size() << endl;
	fm.output << "Total Preemptions: " << preemptions << endl;
}
