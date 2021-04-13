/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/
#ifndef AGENTS_LOADER_H
#define AGENTS_LOADER_H

#include <vector>
#include <string>
#include <utility>
#include "map_loader.h"
#include "tasks_loader.h"

using namespace std;


class Agent{
public:
    int agent_id;
    int start_location;
    int row;
    int col;
    bool isdelivering;
    unsigned int maxtime;
    unsigned int timestep;
    Task *task_sequence;
    vector<int> path;
    
    Agent() {};
    Agent(int start_location, int col, int row, int agent_id, int maxtime);
    ~Agent();
    void Set(int start_location, int col, int row, int agent_id, int maxtime);
};

class AgentsLoader {
public:
    int num_of_agents;
    vector<Agent> agents_all; // agent_all store all the agent and agents_all[i] has agent_id i
    vector< pair<int, int> > initial_locations;
    vector< pair<int, int> > goal_locations;
    
    AgentsLoader(const std::string fname, const MapLoader &ml, int agentsNum);
    AgentsLoader();
    ~AgentsLoader();

    // void printAgentsInitGoal ();
    // pair<int, int> agentStartOrGoalAt(int row, int col);
    // void clearLocationFromAgents(int row, int col);
};
#endif
