#ifndef RM_H
#define RM_H
#include <stdio.h>
#include <vector>
#include <queue>
#include <algorithm>
#include "task_manager.h"
#include "file_manager.h"

#define UNUSED_TASK '+'

using namespace std;

vector<task> set_priority(vector<task> tasks);
void rate_monotonic(int argc, char *argv[]);

#endif
