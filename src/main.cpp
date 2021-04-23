#include <iostream>
#include <string.h>
#include "tasks_loader.h"
#include "agents_loader.h"
#include "LNS.h"
using std::string;

// string agents[5] = {"10", "20", "30", "40", "50"};
string tasks[5] = {"2"};
string agents[5] = {"10"};

int main(int argc, char** argv) {
	for (int j = 0; j < 1; j++) {
		string taskfile = "../Instances/small/kiva-" + tasks[j] + ".task";
		for(int i = 0; i < 1; i++) {
  	    	string mapfile = "../Instances/small/kiva-" + agents[i] + "-500-5.map";
        	string outfile = "../output/offline/" + agents[i] + "-small-" + tasks[j] + ".txt";
			string outStatFile = "../output/offline/small-stats.txt";

			AgentsLoader* al = new AgentsLoader(mapfile);
			TasksLoader* tl = new TasksLoader(taskfile, mapfile);

			LNS lns(*tl, *al,  atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), outfile, outStatFile); // insertion_strategy, removal_strategy, neighborhood_size
			lns.run(atoi(argv[5]), atoi(argv[6])); // maximum ietration time for lns
		}
	}
}