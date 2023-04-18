#ifndef EDF_H
#define EDF_H
#include <stdio.h>
#include <vector>
#include <queue>
#include "task_manager.h"
#include "file_manager.h"
//#include "scheduler.h"

using namespace std;

bool earliest_deadline_first(int argc, char *argv[]);

void combineTasks(vector<task>&, vector<task>&);

void printTasks(vector<task>);
void printTasks(vector<task>, vector<task>);

void quickSort(vector<task>&, int, int);
int partition(vector<task>&, int, int);

void bubbleSort(vector<task>&);

#endif
