/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/

#include <LNS.h>

bool LNS::run(float time_limit, int max_iterations)
{
    start_time = Time::now();
    if (!getInitialSolution()) {
        return false;
    }
    for (Agent agent : al.agents_all) {
        if (agent.task_sequence.empty()) {
            continue;
        }
        else {
            int curr_makespan = calculateMakespan(agent, agent.task_sequence);
            initial_makespan = std::max(curr_makespan, initial_makespan);
            int curr_flowtime = calculateFlowtime(agent, agent.task_sequence);
            initial_flowtime += curr_flowtime;
        }
    }
    initial_runtime = (Time::now() - start_time).count();

    // cout << "Initial makespan = " << initial_makespan << ", "
    //     << "Initial flowtime = " << initial_flowtime << ", "
    //     << "runtime = " << initial_runtime << endl;

    int iterations = 0;
    int best_makespan = initial_makespan;
    while (initial_runtime < time_limit && iterations < max_iterations) {
        iterations++;
        for (int i = 0; i < (int)tl.tasks_all.size(); i++) {
            neighbors[i] = i;
        }
        switch (removal_strategy)
        {
            case 0:
                std::random_shuffle(neighbors.begin(), neighbors.end());
                neighbors.resize(neighborhood_size);
                break;
            case 1:
                generateNeighborsByShawRemoval();
                neighbors.resize(neighborhood_size);
                break;
            case 2:
                generateNeighborsByWorstRemoval();
                neighbors.resize(neighborhood_size);
                break;
            default:
                cout << "Wrong removal strategy" << endl;
                exit(0);
        }
        for (Agent agent : al.agents_all) {
            best_task_sequence[agent.agent_id] = agent.task_sequence;
            for (int i = 0; i < agent.task_sequence.size(); i++) {
                if (std::find(neighbors.begin(), neighbors.end(), tl.tasks_all[agent.task_sequence[i]].task_id) != neighbors.end()) {
                    agent.task_sequence.erase(agent.task_sequence.begin()+i);
                }
            }
        }
        initializeAssignmentHeap();
        for (int i = 0; i < (int)neighbors.size(); i++)
        {
            sortNeighborsByStrategy(insertion_strategy);
            addTaskAssignment();
            updateAssignmentHeap();
        }
        for (Agent agent : al.agents_all) {
            if (agent.task_sequence.empty()) {
                continue;
            }
            else {
                int curr_makespan = calculateMakespan(agent, agent.task_sequence);
                makespan = std::max(curr_makespan, makespan);
                int curr_flowtime = calculateFlowtime(agent, agent.task_sequence);
                flowtime += curr_flowtime;
            }
        }
        runtime = (Time::now() - start_time).count();
        // cout << "Iteration " << iterations << ", " 
        //      << "Neighborhood size " << neighborhood_size << ", "
        //      << "Makespan = " << makespan << ", "
        //      << "Flowtime = " << flowtime << ", "
        //      << "runtime = " << runtime << endl;
        if (makespan < best_makespan) {
            best_task_sequence.clear();
            for (Agent agent : al.agents_all) {
                best_task_sequence[agent.agent_id] =  agent.task_sequence;
            }
        }
        else {
            for (Agent agent : al.agents_all) {
                agent.task_sequence.clear();
                agent.task_sequence = best_task_sequence[agent.agent_id];
            }
        }
    }
}

void LNS::generateNeighborsByShawRemoval()
{   
    int delivery_time = 0;
    int pick_up_time = 0;
    Task random_task = tl.tasks_all[rand() % tl.tasks_all.size()];
    for (Agent agent : al.agents_all) {
        for (int i = 0; i < agent.task_sequence.size(); i++) {
            Task task = tl.tasks_all[agent.task_sequence[i]];
            Task next_task = tl.tasks_all[agent.task_sequence[i+1]];
            if (i == 0) {
                delivery_time += calculateManhattanDistance(agent.start_location, task.pick_up_loc);
                pick_up_time += calculateManhattanDistance(agent.start_location, task.pick_up_loc);
            }
            delivery_time += calculateManhattanDistance(task.pick_up_loc, task.delivery_loc);
            task.pick_up_time = pick_up_time;
            task.delivery_time = delivery_time;
            delivery_time += calculateManhattanDistance(task.delivery_loc, next_task.pick_up_loc);
            if (i == agent.task_sequence.size()-2) {
                delivery_time += calculateManhattanDistance(next_task.pick_up_loc, next_task.delivery_loc);
            }
        }
    }
    for (int i = 0; i < tl.tasks_all.size(); i++) {
        Task task = tl.tasks_all[i];
        task.relatedness = relatedness_weight1 * (calculateManhattanDistance(task.delivery_loc, random_task.delivery_loc)
            + calculateManhattanDistance(task.pick_up_loc, random_task.pick_up_loc)) + 
            + relatedness_weight2 * (std::abs(task.pick_up_time - random_task.pick_up_time) + std::abs(task.delivery_time - random_task.delivery_time));
    }
    quickSort(neighbors, 0, neighbors.size()-1, true, false);

}

void LNS::generateNeighborsByWorstRemoval()
{   
    for (Agent agent : al.agents_all) {
        for (int i = 0; i < agent.task_sequence.size(); i++) {
            Task task = tl.tasks_all[agent.task_sequence[i]];
            vector<int> new_task_sequence = agent.task_sequence;
            new_task_sequence.erase(new_task_sequence.begin()+i);
            task.delta_cost = std::abs(calculateMakespan(agent, agent.task_sequence) - calculateMakespan(agent, new_task_sequence));
        }
    }
    quickSort(neighbors, 0, neighbors.size()-1, true, false);
}


int LNS::calculateMakespan(Agent agent, vector<int> task_sequence)
{
    int makespan = 0;
    for (int i = 0; i < task_sequence.size()-1; i++) {
        Task task = tl.tasks_all[task_sequence[i]];
        Task next_task = tl.tasks_all[task_sequence[i+1]];
        if (i == 0) {
            makespan += calculateManhattanDistance(agent.start_location, task.pick_up_loc);
        }
        makespan += calculateManhattanDistance(task.pick_up_loc, task.delivery_loc);
        makespan += calculateManhattanDistance(task.delivery_loc, next_task.pick_up_loc);
        if (i == task_sequence.size()-2) {
            makespan += calculateManhattanDistance(next_task.pick_up_loc, next_task.delivery_loc);
        }
    }
}

int LNS::calculateFlowtime(Agent agent, vector<int> task_sequence)
{
    int sum_of_release_time = 0;
    int sum_of_delivery_time = 0;
    int delivery_time = 0;
    for (int i = 0; i < task_sequence.size()-1; i++) {
        Task task = tl.tasks_all[task_sequence[i]];
        Task next_task = tl.tasks_all[task_sequence[i+1]];
        sum_of_release_time += task.release_time;
        if (i == 0) {
            delivery_time += calculateManhattanDistance(agent.start_location, task.pick_up_loc);
        }
        delivery_time += calculateManhattanDistance(task.pick_up_loc, task.delivery_loc);
        sum_of_delivery_time += delivery_time;
        delivery_time += calculateManhattanDistance(task.delivery_loc, next_task.pick_up_loc);
        if (i == task_sequence.size()-2) {
            delivery_time += calculateManhattanDistance(next_task.pick_up_loc, next_task.delivery_loc);
            sum_of_delivery_time += delivery_time;
        }
    }
    return sum_of_delivery_time - sum_of_release_time;
}

bool LNS::getInitialSolution()
{
    neighbors.resize(tl.tasks_all.size());
    for (int i = 0; i < (int)tl.tasks_all.size(); i++) {
        neighbors[i] = i;
    }
    initializeAssignmentHeap();
    for (int i = 0; i < (int)neighbors.size(); i++)
    {
        sortNeighborsByStrategy(insertion_strategy);
        addTaskAssignment();
        updateAssignmentHeap();
    }
    
}

void LNS::initializeAssignmentHeap()
{
    for (auto task : neighbors) {
        int heap_num = 0;
        for (auto agent : al.agents_all) {
            for (int i = 0; i < agent.task_sequence.size()+1; i++) {
                int temp_cost = calculateMarginalCost(tl.tasks_all[task], agent.task_sequence, i, agent);
                tl.tasks_all[task].ta[heap_num] = &TaskAssignment(agent.agent_id, i, temp_cost);
                tl.tasks_all[task].assignment_heap.push(tl.tasks_all[task].ta[heap_num]);
                heap_num++;
            }
        }
    }
}

int LNS::calculateMarginalCost(Task task, vector<int> task_sequence, int pos, Agent Agent)
{
    if (pos == 0) {
        return calculateManhattanDistance(task.pick_up_loc, Agent.start_location) 
            + calculateManhattanDistance(task.pick_up_loc, task.delivery_loc)
            + calculateManhattanDistance(task.delivery_loc, tl.tasks_all[task_sequence[1]].pick_up_loc);
    }
    else if (pos == task_sequence.size()) {
        return calculateManhattanDistance(tl.tasks_all[task_sequence[task_sequence.size()]].delivery_loc, task.pick_up_loc) 
            + calculateManhattanDistance(task.pick_up_loc, task.delivery_loc);
    }
    else {
        return calculateManhattanDistance(tl.tasks_all[task_sequence[pos-1]].delivery_loc, task.pick_up_loc) 
            + calculateManhattanDistance(task.pick_up_loc, task.delivery_loc)
            + calculateManhattanDistance(task.delivery_loc, tl.tasks_all[task_sequence[pos+1]].pick_up_loc);
    }
}

int LNS::calculateManhattanDistance(int loc1, int loc2)
{
    // transfer to x, y index
    int loc1_x = 0;
    int loc1_y = 0;
    int loc2_x = 0;
    int loc2_y = 0;
    return std::abs(loc1_x - loc2_x) + std::abs(loc1_y - loc2_y);
}


void LNS::sortNeighborsByStrategy(int insertion_strategy)
{
    if (insertion_strategy = 0) {
        std::random_shuffle(neighbors.begin(), neighbors.end());
    }
    else if (insertion_strategy = 1) {
        quickSort(neighbors, 0, neighbors.size()-1, false, false);
    }
    else {
        quickSort(neighbors, 0, neighbors.size()-1, true, false);
    }
}

void LNS::quickSort(vector<int>& task_order, int low, int high, bool regret, bool insert)
{
    if (low >= high) {
        return;
    }
    int pivot = task_order[high];
    int i = low;
    for (int j = low; j <= high - 1; j++) {
        if (tl.compareTask(tl.tasks_all[task_order[j]], tl.tasks_all[pivot], insert, insertion_strategy, removal_strategy)) {
            std::swap(task_order[i], task_order[j]);
            i++;
        }
    }
    std::swap(task_order[i], task_order[high]);
    quickSort(task_order, low, i-1, regret, insert);
    quickSort(task_order, i+1, high, regret, insert);
}

void LNS::addTaskAssignment()
{
    Task t = tl.tasks_all[neighbors[0]];
    int pos = t.assignment_heap.top()->pos;
    updated_agent = t.assignment_heap.top()->agent;
    al.agents_all[updated_agent].task_sequence.insert(al.agents_all[updated_agent].task_sequence.begin()+pos, t.task_id);
    neighbors.erase(neighbors.begin());
}

void LNS::updateAssignmentHeap()
{
    for (auto task : neighbors) {
        for (int i = 0; i < tl.tasks_all[task].ta.size(); i++) {
            if (tl.tasks_all[task].ta[i]->agent == updated_agent) {
                tl.tasks_all[task].ta.erase(tl.tasks_all[task].ta.begin()+i);
            }
        }
        for (int i = 0; i < al.agents_all[updated_agent].task_sequence.size()+1; i++) {
            int temp_cost = calculateMarginalCost(tl.tasks_all[task], al.agents_all[updated_agent].task_sequence, i, al.agents_all[updated_agent]);
            tl.tasks_all[task].ta.push_back(&TaskAssignment(al.agents_all[updated_agent].agent_id, i, temp_cost));
            tl.tasks_all[task].assignment_heap.push(*tl.tasks_all[task].ta.end());
        }
    }
}