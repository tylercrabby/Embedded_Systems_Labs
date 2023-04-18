#include "file_manager.h"

FileManager::FileManager(int argc, char *argv[]) {			
	string temp_str;
	vector<string> lines;
	num_tasks = 0;
	num_a_tasks = 0;

	// Check to make sure the user gave an input and an output file name to be used.
	if(argc != 3){
		cout << "\n!!!!!!!!!!\n";
		cout << "This program requires a valid input text file, and some name to call the output file. \n";
		cout << "Please try to run the program again.\n\n";
		exit(EXIT_FAILURE);
	}

	// Open a file frpm the user to get our scheduling constraints.
	input.open(argv[1]);

	// Did the user actually specify a valid file name?
	if(!input.good()){
		cout << "\n!!!!!!!!!!\n";
		cout << "That filename: " << argv[1] << ", was either not valid or in the working directory.\n";
		cout << "Please try again with a valid filename.\n\n";
		exit(EXIT_FAILURE);
	}

	// If the input filename was valid, create an output file to write to.
	output.open(argv[2], ios::out); 

	// Was that file successfully created?
	if(!output.good()){
		cout << "\n!!!!!!!!!!\n";
		cout << "There was a problem creating your " << argv[2] << " file.\n";
		cout << "Please try again with a valid filename.\n\n";
		exit(EXIT_FAILURE);
	}

	// Parse periodic data header
	input >> num_tasks;
	input >> sim_time;
	getline(input, temp_str);

	// Parse periodic data
	for (int i = 0; i < num_tasks; i++) {
		getline(input, temp_str);
		lines.push_back(temp_str);
	}

	for (int i = 0; i < num_tasks; i++) {
		stringstream ss(lines[i]);
		task temp_t;
		temp_t.task_state = 0;
		temp_t.is_a = false;
		char comma;

		ss >> temp_t.name >> comma >> temp_t.exe_time >> comma >> temp_t.period;
		tasks.push_back(temp_t);
	}
	
	// Parse aperiodic data header
	input >> num_a_tasks;
	getline(input, temp_str);
	lines.clear();

	// Parse aperiodic data
	for (int i = 0; i < num_a_tasks; i++) {
		getline(input, temp_str);
		lines.push_back(temp_str);
	}

	for (int i = 0; i < num_a_tasks; i++) {
		stringstream ss(lines[i]);
		task temp_t;
		temp_t.task_state = 0;
		temp_t.is_a = true;
		char comma;

		ss >> temp_t.name >> comma >> temp_t.exe_time >> comma >> temp_t.period;
		a_tasks.push_back(temp_t);
	}
}

int FileManager::get_sim_time() {
	return sim_time;
}

int FileManager::get_num_tasks() {
	return num_tasks;
}

int FileManager::get_num_a_tasks() {
	return num_a_tasks;
}

vector<task> FileManager::get_tasks() {
	return tasks;
}

vector<task> FileManager::get_a_tasks() {
	return a_tasks;
}

FileManager::~FileManager() {
	output.close();
}
