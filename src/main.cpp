#include <iostream>
#include <string.h>
#include "tasks_loader.h"
#include "agents_loader.h"
#include "LNS.h"
using std::string;

string agents[5] = {"60", "90", "120", "150", "180"};

int main(int argc, char** argv) {

	for(int i = 0; i < 1; i++) {
		string taskfile = "../Instances/small/kiva-2.task";
  	    string mapfile = "../Instances/small/kiva-10-500-5.map"; 
		//    + agents[i] + ".map";
		string outfile = "../output/offline/10-small.txt";
        // string outfile = "../output/offline/" + agents[i] + "-large" + ".txt";

		AgentsLoader* al = new AgentsLoader(mapfile);
		// AgentsLoader al(mapfile);
		// TasksLoader tl(taskfile);
		TasksLoader* tl = new TasksLoader(taskfile);
		cout << al->agents_all.size()<< endl;
		for (int i = 0; i < al->agents_all.size(); i++) {
			cout <<  al->agents_all[i].agent_id << " " << al->agents_all[i].start_location << endl;
		}
		// for (int i = 0; i < tl.tasks_all.size(); i++) {
		// 	cout <<  tl.tasks_all[i].pick_up_loc << " " << tl.tasks_all[i].delivery_loc<< endl;
		// }
		// LNS lns(*tl, *al,  atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), outfile); // insertion_strategy, removal_strategy, neighborhood_size
		// lns.run(atoi(argv[4]), atoi(argv[5])); // maximum ietration time for lns
	}
}