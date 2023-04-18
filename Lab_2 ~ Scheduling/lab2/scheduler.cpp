/*
Carter Nettesheim
Tyler Crabb
Lab 2 ~ Rate Monotonic and Earliest Deadline First Scheduling
Dr. Phillips' ECE5780 Embedded Real-Time Systems
*/

#include "scheduler.h"

/*
This big bad scheduling program takes an input file of tasks, their runtimes, and release windows and creates a schedule to run them all by.
The schedules will be written to an output text file that the user will name.
*/
int main(int argc, char *argv[]) {
	bool alg;

	cout << "run\nRM: 0\nEDF: 1\n" << endl;
	cin >> alg;

	if (alg) {
		earliest_deadline_first(argc, argv);
		cout << "EDF finished successfully" << endl;
	}
	else {
		rate_monotonic(argc, argv);
		cout << "RM finished successfully" << endl;
	}	
	
	return 0;
}
