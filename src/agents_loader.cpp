/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/

#include <string>
#include <cstring>
#include <iostream>
#include <cassert>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <utility>
#include <ctime>
#include "agents_loader.h"

using namespace boost;
using namespace std;

int RANDOM_WALK_STEPS = 100000;

Agent::Agent() {};

Agent::Agent(int start_location, int col, int row, int agent_id, int maxtime)
    :start_location(start_location), col(col), row(row),
    agent_id(agent_id), timestep(1), maxtime(maxtime),task_sequence(NULL), isdelivering(false)
{
    for (int i = 0; i < maxtime; i++)
    {
        path.push_back(start_location);  //stay
    }
};

Agent::~Agent(){}

void Agent::Set(int start_location, int col, int row, int agent_id, int maxtime)
{
    this->start_location = start_location;
    this->col = col;
    this->row = row;
    this->agent_id = agent_id;
    this->timestep = 0;
    this->task_sequence = NULL;
    this->isdelivering = false;
    this->maxtime = maxtime;
    for (int i = 0; i < maxtime; i++)
    {
        path.push_back(start_location);  //stay
    }
};

AgentsLoader::AgentsLoader() {
  num_of_agents = 0;
}

AgentsLoader::AgentsLoader(string fname, const MapLoader &ml, int agentsNum = 0){
    string line;
    ifstream myfile (fname.c_str());
    if(myfile.is_open())
    {
        getline (myfile,line);
        char_separator<char> sep(",");
        tokenizer< char_separator<char> > tok(line, sep);
        tokenizer< char_separator<char> >::iterator beg=tok.begin();
        int rows = atoi ((*beg).c_str());
        beg++;
        int cols = atoi ((*beg).c_str());

        stringstream ss;
        int num_of_agents = 0
        getline(myfile, line);
        getline(myfile, line);
        ss << line;
        ss >> num_of_agents;
        this->num_of_agents = num_of_agents;
        this->agents_all.resize(num_of_agents);

        int maxtime;
        ss.clear();
        getline(myfile, line);
        ss << line;
        ss >> maxtime;

        int ag = 0;
        for (int i = 1; i<rows - 1; i++)
        {
            getline(myfile, line);
            for (int j = 1; j<cols - 1; j++)
            {
                if (line[j - 1] == 'r') //robot start location
                {
                    this->agents_all[ag].Set(i*col + j, cols, rows, n, maxtime);
                    ag++;
                }
            }
        }
        // for (int i=0; i<num_of_agents; i++)
        // {
        //     getline (myfile, line);
        //     tokenizer< char_separator<char> > col_tok(line, sep);
        //     tokenizer< char_separator<char> >::iterator c_beg=col_tok.begin();
        //     pair<int,int> curr_pair;
            
        //     // read start [row,col] for agent i
        //     curr_pair.first = atoi ( (*c_beg).c_str() );
        //     c_beg++;
        //     curr_pair.second = atoi ( (*c_beg).c_str() );
        //     this->initial_locations.push_back(curr_pair);
            
        //     // read goal [row,col] for agent i
        //     c_beg++;
        //     curr_pair.first = atoi ( (*c_beg).c_str() );
        //     c_beg++;
        //     curr_pair.second = atoi ( (*c_beg).c_str() );
        //     this->goal_locations.push_back(curr_pair);
        // }
    }
    else{
        cerr << "File" << fname << "not found." << endl;
        return;
    }
    myfile.close();
}

AgentsLoader::~AgentsLoader() {
  // vectors are on stack, so they are freed automatically
}

// void AgentsLoader::printAgentsInitGoal () {
//     cout << "AGENTS:" << endl;
//     for (int i=0; i<num_of_agents; i++)
//     {
//         cout << "Agent" << i << " : I=(" << initial_locations[i].first << "," << initial_locations[i].second << ") ; G=(" <<
//         goal_locations[i].first << "," << goal_locations[i].second << ")" << endl;
//     }
//     cout << endl;
// }

// returns the agents' ids if they occupy [row,col] (first for start, second for goal)
// pair<int, int> AgentsLoader::agentStartOrGoalAt(int row, int col) {
//     int f = -1;
//     int s = -1;
//     for (vector< pair<int, int> >::iterator it = initial_locations.begin(); it != initial_locations.end(); ++it)
//     if ( it->first == row && it->second == col )
//         f = std::distance(initial_locations.begin(), it);
//     for (vector< pair<int, int> >::iterator it = goal_locations.begin(); it != goal_locations.end(); ++it)
//     if ( it->first == row && it->second == col )
//         s = std::distance(goal_locations.begin(), it);
//     return make_pair(f, s);
// }

// void AgentsLoader::clearLocationFromAgents(int row, int col) {
//     pair<int, int> idxs = agentStartOrGoalAt(row, col);
//     if ( idxs.first != -1 ) {  // remove the agent who's start is at [row,col]
//         initial_locations.erase( initial_locations.begin() + idxs.first );
//         goal_locations.erase ( goal_locations.begin() + idxs.first );
//         num_of_agents--;
//     }
//     idxs = agentStartOrGoalAt(row, col);
//     if ( idxs.second != -1 ) {  // remove the agent who's goal is at [row,col]
//         initial_locations.erase( initial_locations.begin() + idxs.second );
//         goal_locations.erase( goal_locations.begin() + idxs.second );
//         num_of_agents--;
//     }
}



