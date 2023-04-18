#ifndef SCHEDULER_H
#define SCHEDULER_H
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
    int state;

} task;

const int IMPLICIT_DEADLINE = 500;
string stateName[] = {"unreleased", "released", "running", "prempted", "completed"};

#endif