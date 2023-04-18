#include "task_manager.h"

// check to see if one task 1 is less than task 2
bool compare_task(const task t1, const task t2) {
	return t1.period < t2.period;
}

// check to see if 2 tasks are equal
bool is_task_equal(const task t1, const task t2) {
	return t1.name == t2.name;
}

// release_tasks releases periodic tasks when their release time has come
string release_tasks(queue<task>* q, vector<task> tasks, int time) {
	string result = "";
	if (time == 0) {
		for (int i = 0; i < tasks.size(); i++) {
			tasks[i].task_state = tasks[i].period + time;
			q->push(tasks[i]);
			result += "Time:\t" + to_string(time) + "\t\tperiodic task pushed:\t";
			result += tasks[i].name;
			result += " deadline: " + to_string(tasks[i].task_state) + "\n";
		}
	}
	else {
		for (int i = 0; i < tasks.size(); i++) {
			if (time % tasks[i].period == 0) {
				tasks[i].task_state = tasks[i].period + time;
				q->push(tasks[i]);
				result += "Time:\t" + to_string(time) + "\t\tperiodic task pushed:\t";
				result += tasks[i].name;
				result += " deadline: " + to_string(tasks[i].task_state) + "\n";
			}
		}
	}
	return result;
}

// release_a_tasks releases aperiodic tasks when their release time has come
string release_a_tasks(queue<task>* q, vector<task> tasks, int time) {
	string result = "";
	if (time != 0) {
		for (int i = 0; i < tasks.size(); i++) {
			if (time == tasks[i].period) {
				tasks[i].task_state = IMPLICIT_DEADLINE + time;
				q->push(tasks[i]);
				result += "Time:\t" + to_string(time) + "\t\taperiodic task pushed:\t";
				result += tasks[i].name; 
				result += " deadline: " + to_string(tasks[i].task_state) + "\n";
			}
		}
	}
	return result;
}

// checks the deadlines of the released periodic tasks
string check_deadline(task cur_task, queue<task> q, int time, int* missed) {
	string result = "";
	if (cur_task.task_state == time && !q.empty()) {
		result += "Time:\t" + to_string(time) + "\t\tTask " + cur_task.name + " missed its deadline\n";
		*missed += 1;
	}
	while (!q.empty()) {
		if (q.front().task_state == time) {
			result += "Time:\t" + to_string(time) + "\t\tTask " + q.front().name + " missed its deadline\n";
			*missed += 1;
		}
		q.pop();
	}
	return result;
}

// checks the deadlines of the release aperiodic tasks
string check_a_deadline(queue<task> q, int time, int* missed) {
	string result = "";
	while (!q.empty()) {
		if (q.front().task_state == time) {
			result += "Time:\t" + to_string(time) + "\t\tTask " + q.front().name + " missed its deadline\n";
			*missed += 1;
		}
		q.pop();
	}
	return result;
}
