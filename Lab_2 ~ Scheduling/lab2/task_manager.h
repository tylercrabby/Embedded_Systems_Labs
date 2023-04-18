#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>

using namespace std;

typedef struct {
	char name;
	int exe_time;
	int period;

	// We need a variable to track which state this task has been in.
	int task_state;
	bool is_a;
} task;

const int IMPLICIT_DEADLINE = 500;

bool compare_task(const task t1, const task t2);
bool is_task_equal(const task t1, const task t2);
string release_tasks(queue<task>* q, vector<task> tasks, int time);
string release_a_tasks(queue<task>* q, vector<task> tasks, int time);
string check_deadline(task cur_task, queue<task> q, int time, vector<char>* missed);
string check_a_deadline(queue<task> q, int time, vector<char>* missed);

#endif
