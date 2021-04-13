#include <iostream>
#include <string.h>
#include "LNS.h"
#include "map_loader.h"
#include "tasks_loader.h"
#include "agents_loader.h"
using std::string;

string agents[5] = {"60", "90", "120", "150", "180"};

int main(int argc, char** argv) {

	for(int i=0; i< 5; i++){
		string taskfile = "./Instances/large/kiva-2000.task";
  	    string mapfile = "./Instances/large/kiva-" + agents[i] + ".map";
        string outfile = "./output/offline/" + agents[i] + "-large" + ".txt";

		AgentsLoader* al = new AgentsLoader();
		TasksLoader* tl = new TasksLoader(taskfile);
		MapLoder* ml = new MapLoder(mapfile);
  
		LNS lns(*tl, *al,  atoi(argv[0]), atoi(argv[1]), atoi(argv[2]), outfile); // insertion_strategy, removal_strategy, neighborhood_size
		lns.run(atoi(argv[3]), atoi(argv[4])); // maximum ietration time for lns
	}
}