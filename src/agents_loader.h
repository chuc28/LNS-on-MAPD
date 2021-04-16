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
    
    AgentsLoader(const std::string fname);
    AgentsLoader();
    ~AgentsLoader();
};
#endif
