#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include "task_manager.h"

using namespace std;

class FileManager {
	private:
		ifstream input;

		int num_tasks;
		int num_a_tasks;
		int sim_time;

		vector<task> tasks;
		vector<task> a_tasks;

	public:
		ofstream output;

		FileManager(int argc, char *argv[]);
		int get_sim_time();
		int get_num_tasks();
		int get_num_a_tasks();
		vector<task> get_tasks();
		vector<task> get_a_tasks();
		~FileManager();
};

#endif
