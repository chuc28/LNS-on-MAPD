/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <vector>
#include "agents_loader.h"

using namespace boost;
using namespace std;


Agent::Agent(int start_location, int agent_id)
    : start_location(start_location), agent_id(agent_id)
{};

Agent::~Agent(){}

void Agent::Set(int start_location, int agent_id)
{
    this->start_location = start_location;
    this->agent_id = agent_id;
    // this->task_sequence = NULL;
};

AgentsLoader::AgentsLoader() 
{
    num_of_agents = 0;
}

AgentsLoader::AgentsLoader(string fname)
{
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
        this->map_cols = cols;
        this->map_rows = rows;

        std::stringstream ss;
        int num_of_agents = 0;
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
        for (int i = 1; i < rows - 1; i++)
        {
            getline(myfile, line);
            for (int j = 1; j < cols - 1; j++)
            {
                if (line[j - 1] == 'r') //robot start location
                {
                    this->agents_all[ag].Set(i*cols + j, ag);
                    ag++;
                }
            }
        }
        this->agents_all.resize(ag); // double check this
    }
    else{
        cerr << "Map file " << fname << " not found." << endl;
        return;
    }
    myfile.close();
}

AgentsLoader::~AgentsLoader() {
  // vectors are on stack, so they are freed automatically
}