#include <vector>
using std::vector;


struct Agent
{
    vector<int> task_sequence;
    int agent_id;
    int start_location;
};

class AgentsLoader{
public:
    vector<Agent> agents_all;


};