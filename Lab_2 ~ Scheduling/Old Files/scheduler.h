#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include "task_manager.h"
#include "file_manager.h"
#include "rm.h"
#include "edf.h"

using namespace std;

string task_state[] = {"unreleased", "released", "running", "prempted", "completed"};

#endif

