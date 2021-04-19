/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/
#ifndef AGENTS_LOADER_H
#define AGENTS_LOADER_H

#include <vector>
#include <string>
#include <utility>
#include "tasks_loader.h"

using namespace std;
struct Endpoint{
     int loc;
     int id;
};

class Agent{
public:
    int agent_id;
    int start_location;
    vector<int> task_sequence;
    int task_sequence_makespan;
    
    Agent() {};
    Agent(int start_location, int agent_id);
    ~Agent();
    void Set(int start_location, int agent_id);
};

class AgentsLoader {
public:
    int num_of_agents;
    int map_cols;
    int map_rows;
    vector<Agent> agents_all; // agent_all store all the agent and agents_all[i] has agent_id i
    int num_of_endpoints;
    vector<Endpoint> endpoints;
    
    int calculateManhattanDistance(int loc1, int loc2);
    AgentsLoader(const std::string fname);
    AgentsLoader();
    ~AgentsLoader();
};
#endif
