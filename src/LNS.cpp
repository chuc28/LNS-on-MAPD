/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/

#include "LNS.h"

bool LNS::run(int time_limit, int max_iterations)
{
    start_time = Time::now();
    srand(time(NULL));
    if (!getInitialSolution()) {
        return false;
    }
    for (Agent& agent : al.agents_all) {
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
    initial_runtime = ((fsec)(Time::now() - start_time)).count();

    cout << "Initial makespan = " << initial_makespan << ", "
        << "Initial flowtime = " << initial_flowtime << ", "
        << "runtime = " << initial_runtime << endl;

    int iterations = 0;
    int best_makespan = initial_makespan;
    int best_flowtime = initial_flowtime;
    // srand(time(0));

    while (((fsec)(Time::now()- start_time)).count() < time_limit) {
        high_resolution_clock::time_point curr_time = Time::now();
        iterations++;
        neighbors.clear();
        for (int i = 0; i < (int)tl.tasks_all.size(); i++) {
            neighbors.push_back(tl.tasks_all[i].task_id);
        }
        int max_agent_id = 0;
        switch (removal_strategy)
        {
            case 0:
                std::shuffle(neighbors.begin(), neighbors.end(), default_random_engine(rand()));
                neighbors.resize(neighborhood_size);
                break;
            case 1:
                generateNeighborsByShawRemoval();
                neighbors.resize(neighborhood_size);
                // neighbors.resize(neighborhood_size / pow(2, std::min(5, iterations / 100)));
                // neighbors.resize(int(tl.tasks_all.size()*r));
                break;
            case 2:
                max_agent_id = generateNeighborsByWorstRemoval();
                // neighbors.resize(rand() % al.agents_all[max_agent_id].task_sequence.size());
                neighbors.resize(neighborhood_size);
                break;
            default:
                cout << "Wrong removal strategy" << endl;
                exit(0);
        }
        

        for (Agent& agent : al.agents_all) {
            curr_task_sequence[agent.agent_id-1] = agent.task_sequence;
            for (int i : neighbors) {
                agent.task_sequence.erase(std::remove(agent.task_sequence.begin(), agent.task_sequence.end(), i), agent.task_sequence.end());
            }
        }

        initializeAssignmentHeap();
        while (neighbors.size())
        {
            sortNeighborsByStrategy(lns_insertion_strategy);
            addTaskAssignment();
            updateAssignmentHeap();
        }
        int makespan = 0;
        int flowtime = 0;
        for (Agent& agent : al.agents_all) {
            if (agent.task_sequence.empty()) {
                continue;
            }
            else {
                int curr_makespan = agent.task_sequence_makespan;
                makespan = std::max(curr_makespan, makespan);
                int curr_flowtime = calculateFlowtime(agent, agent.task_sequence);
                flowtime += curr_flowtime;
            }
        }
        runtime = ((fsec)(Time::now() - curr_time)).count();
        // cout << "Iteration " << iterations << ", " 
        //      << "Neighborhood size " << neighborhood_size << ", "
        //      << "Makespan = " << makespan << ", "
        //      << "Flowtime = " << flowtime << ", "
        //      << "runtime = " << runtime << endl;
        if (makespan < best_makespan) {
            best_task_sequence.clear();
            for (Agent& agent : al.agents_all) {
                best_task_sequence[agent.agent_id-1] =  agent.task_sequence;
            }
            best_makespan = makespan;
            best_flowtime = flowtime;
        }
        else {
            for (Agent& agent : al.agents_all) {
                agent.task_sequence.clear();
                agent.task_sequence = curr_task_sequence[agent.agent_id-1];
            }
        }
    }
    runtime = ((fsec)(Time::now() - start_time)).count();
    // cout << "LNS improves makepsan from " << initial_makespan 
    //     << " to " << best_makespan << endl;
    // cout << "LNS improves flowtime from " << initial_flowtime
    //     << " to " << best_flowtime << endl;
    // cout << "Runtime " << runtime << " Iteration " << iterations << endl;

    // save output as txt file
    int agent_num = al.agents_all.size();
    std::ofstream outFile(outfile);
    for (int i = 0; i < agent_num; i++) {
        outFile << al.agents_all[i].agent_id << " " << al.agents_all[i].start_location << "\n";
        for (int e : al.agents_all[i].task_sequence) {
            outFile << e + agent_num  << " " << tl.tasks_all[e-1].release_time << " " << tl.tasks_all[e-1].pick_up_loc << " " << tl.tasks_all[e-1].delivery_loc << "\n";
        }
    }
    outFile.close();

    outFile.open(outStatFile, std::ios_base::app);
    outFile << "=== Task Frequency " << tl.frequency << " Agents Number " << agent_num << "\n";
    outFile << "LNS improves makepsan from " << initial_makespan << " to " << best_makespan << "\n";
    outFile << "LNS improves flowtime from " << initial_flowtime << " to " << best_flowtime << "\n";
    outFile << "Runtime " << runtime << " Iteration " << iterations << "\n";
    outFile.close();
    return 0;
}

void LNS::generateNeighborsByShawRemoval()
{   
    Task& random_task = tl.tasks_all[rand() % tl.tasks_all.size()];

    // Calculate delivery and pickup time for each task
    for (Agent& agent : al.agents_all) {
        int delivery_time = 0;
        int pick_up_time = 0;
        for (int i = 0; i < agent.task_sequence.size(); i++) {
            Task& task = tl.tasks_all[agent.task_sequence[i]-1];
            if (i == 0) {
                pick_up_time += al.calculateManhattanDistance(agent.start_location, task.pick_up_loc);
            }
            task.pick_up_time = std::max(pick_up_time, task.release_time);
            task.delivery_time = task.pick_up_time + al.calculateManhattanDistance(task.pick_up_loc, task.delivery_loc);
            delivery_time = task.delivery_time;
            if (i != agent.task_sequence.size()-1) {
                Task& next_task = tl.tasks_all[agent.task_sequence[i+1]-1];
                pick_up_time = task.delivery_time + al.calculateManhattanDistance(task.delivery_loc, next_task.pick_up_loc);
            }
        }
    }
    for (int i = 0; i < tl.tasks_all.size(); i++) {
        Task& task = tl.tasks_all[i];
        task.relatedness = relatedness_weight1 * (al.calculateManhattanDistance(task.delivery_loc, random_task.delivery_loc)
            + al.calculateManhattanDistance(task.pick_up_loc, random_task.pick_up_loc)) + 
            + relatedness_weight2 * (std::abs(task.pick_up_time - random_task.pick_up_time) + std::abs(task.delivery_time - random_task.delivery_time));
    }
    quickSort(neighbors, 0, neighbors.size()-1, false, insertion_strategy, removal_strategy);

}

int LNS::generateNeighborsByWorstRemoval()
{   
    for (Agent& agent : al.agents_all) {
        for (int i = 0; i < agent.task_sequence.size(); i++) {
            Task& task = tl.tasks_all[agent.task_sequence[i]];
            vector<int> new_task_sequence = agent.task_sequence;
            new_task_sequence.erase(new_task_sequence.begin()+i);
            task.delta_cost = calculateMakespan(agent, agent.task_sequence) - calculateMakespan(agent, new_task_sequence);
        }
    }
    // int max_agent_id = 0;
    // int max_ta_len = 0;
    // for (Agent& agent : al.agents_all) {
    //     if (agent.task_sequence.size() > max_ta_len) {
    //         max_agent_id = agent.agent_id;
    //         max_ta_len = agent.task_sequence.size();
    //     }
    //     for (int i = 0; i < agent.task_sequence.size(); i++) {
    //         Task& task = tl.tasks_all[agent.task_sequence[i]];
    //     //     vector<int> new_task_sequence = agent.task_sequence;
    //     //     new_task_sequence.erase(new_task_sequence.begin()+i);
    //         task.delta_cost = agent.task_sequence_makespan;
    //     }
    // }
    quickSort(neighbors, 0, neighbors.size()-1, false, insertion_strategy, removal_strategy);
    return 0;
}

int LNS::calculateMakespan(Agent agent, vector<int> task_sequence)
{
    int makespan = 0;
    for (int i = 0; i < task_sequence.size(); i++) {
        Task& task = tl.tasks_all[task_sequence[i]-1];
        if (i == 0) {
            makespan += al.calculateManhattanDistance(agent.start_location, task.pick_up_loc);
            makespan = std::max(task.release_time, makespan); // same as TA-Prioritized
        }
        makespan += al.calculateManhattanDistance(task.pick_up_loc, task.delivery_loc);
        if (i != task_sequence.size()-1 ) {
            Task& next_task = tl.tasks_all[task_sequence[i+1]-1];
            makespan += al.calculateManhattanDistance(task.delivery_loc, next_task.pick_up_loc);
            makespan = std::max(next_task.release_time, makespan);
        }
    }
    return makespan;
}

int LNS::calculateFlowtime(Agent agent, vector<int> task_sequence)
{
    int sum_of_release_time = 0;
    int sum_of_delivery_time = 0;
    int delivery_time = 0;
    for (int i = 0; i < task_sequence.size()-1; i++) {
        Task& task = tl.tasks_all[task_sequence[i]-1];
        Task& next_task = tl.tasks_all[task_sequence[i+1]-1];
        sum_of_release_time += task.release_time;
        if (i == 0) {
            delivery_time += al.calculateManhattanDistance(agent.start_location, task.pick_up_loc);
            delivery_time = std::max(delivery_time, task.release_time);
        }
        delivery_time += al.calculateManhattanDistance(task.pick_up_loc, task.delivery_loc);
        // cout << delivery_time << endl;
        sum_of_delivery_time += delivery_time;
        delivery_time += al.calculateManhattanDistance(task.delivery_loc, next_task.pick_up_loc);
        delivery_time = std::max(delivery_time, next_task.release_time);
        if (i == task_sequence.size()-2) {
            delivery_time += al.calculateManhattanDistance(next_task.pick_up_loc, next_task.delivery_loc);
            sum_of_delivery_time += delivery_time;
        }
    }
    sum_of_release_time += tl.tasks_all[task_sequence.size()-1].release_time;
    // cout << "======" << endl;
    return sum_of_delivery_time - sum_of_release_time;
}

bool LNS::getInitialSolution()
{
    neighbors.resize(tl.tasks_all.size());
    for (int i = 0; i < (int)tl.tasks_all.size(); i++) {
        neighbors[i] = tl.tasks_all[i].task_id;
    }
    initializeAssignmentHeap();
    while (neighbors.size())
    {
        sortNeighborsByStrategy(insertion_strategy);
        addTaskAssignment();
        updateAssignmentHeap();
    }
    return true;
}

void LNS::initializeAssignmentHeap()
{
    for (int task : neighbors) {
        tl.tasks_all[task-1].ta.clear();
        tl.tasks_all[task-1].assignment_heap.clear();
        for (auto& agent : al.agents_all) {
            if (agent.task_sequence.size() == 0) {
                agent.task_sequence_makespan = 0;
            }
            else {
                agent.task_sequence_makespan = calculateMakespan(agent, agent.task_sequence);
            }
            for (int i = 0; i < agent.task_sequence.size()+1; i++) {
                vector<int> new_task_sequence = agent.task_sequence;
                new_task_sequence.insert(new_task_sequence.begin()+i, task);
                int temp_cost = calculateMakespan(agent, new_task_sequence) - agent.task_sequence_makespan;
                Key agent_pos_pair(agent.agent_id, i);
                TaskAssignment task_assignment(agent.agent_id, i, temp_cost);
                handle_t handle = tl.tasks_all[task-1].assignment_heap.push(task_assignment);
                tl.tasks_all[task-1].ta.insert(std::make_pair(agent_pos_pair, handle));
            }
        }
    }
}

// int LNS::calculateManhattanDistance(int loc1, int loc2)
// {
//     // transfer to x, y index
//     int loc1_x = loc1 / al.map_cols + 1;
//     int loc1_y = loc1 % al.map_cols + 1;
//     int loc2_x = loc2 / al.map_cols + 1;
//     int loc2_y = loc2 % al.map_cols + 1;
//     return std::abs(loc1_x - loc2_x) + std::abs(loc1_y - loc2_y);
// }

void LNS::sortNeighborsByStrategy(int curr_insertion_strategy)
{
    if (curr_insertion_strategy == 0) {
        std::shuffle(neighbors.begin(), neighbors.end(), default_random_engine(rand()));
    }
    else if (curr_insertion_strategy == 1 || curr_insertion_strategy == 2) {
        quickSort(neighbors, 0, neighbors.size()-1, true, curr_insertion_strategy, removal_strategy);
    }
    else {
        cout << "Wrong insertion strategy" << endl;
        exit(0);
    }
}

void LNS::quickSort(vector<int>& task_order, int low, int high, bool insert, int curr_insertion_strategy, int removal_strategy)
{
    if (low >= high) {
        return;
    }
    int pivot = task_order[high];
    int i = low;
    for (int j = low; j <= high - 1; j++) {
        if (tl.compareTask(tl.tasks_all[task_order[j]-1], tl.tasks_all[pivot-1], insert, curr_insertion_strategy, removal_strategy)) {
            std::swap(task_order[i], task_order[j]);
            i++;
        }
    }
    std::swap(task_order[i], task_order[high]);
    quickSort(task_order, low, i-1, insert, curr_insertion_strategy, removal_strategy);
    quickSort(task_order, i+1, high, insert, curr_insertion_strategy, removal_strategy);
}

void LNS::addTaskAssignment()
{
    Task& t = tl.tasks_all[neighbors[0]-1];
    removed_task = t.task_id;
    int pos = t.assignment_heap.top().pos;
    updated_agent = t.assignment_heap.top().agent;
    // cout << updated_agent << endl;
    al.agents_all[updated_agent-1].task_sequence.insert(al.agents_all[updated_agent-1].task_sequence.begin()+pos, t.task_id);
    neighbors.erase(neighbors.begin());
}

void LNS::updateAssignmentHeap()
{
    int curr_cost = 0;
    int temp_size = al.agents_all[updated_agent-1].task_sequence.size();
    if (temp_size > 0) {
        curr_cost = calculateMakespan(al.agents_all[updated_agent-1], al.agents_all[updated_agent-1].task_sequence);
    }
    al.agents_all[updated_agent-1].task_sequence_makespan = curr_cost;
    for (auto task : neighbors) {
        for (int i = 0; i < temp_size + 1; i++) {
            vector<int> new_task_sequence = al.agents_all[updated_agent-1].task_sequence;
            new_task_sequence.insert(new_task_sequence.begin()+i, task);
            int temp_cost = calculateMakespan(al.agents_all[updated_agent-1], new_task_sequence) - curr_cost;
            Key agent_pos_pair(updated_agent, i);
            if (tl.tasks_all[task-1].ta.find(agent_pos_pair) != tl.tasks_all[task-1].ta.end()) {
                handle_t handle = tl.tasks_all[task-1].ta[agent_pos_pair];
                if ((*handle).insertion_cost != temp_cost) {
                    (*handle).insertion_cost = temp_cost;
                    tl.tasks_all[task-1].assignment_heap.update(handle);
                }
            } else {
                TaskAssignment task_assignment(updated_agent, i, temp_cost);
                handle_t handle = tl.tasks_all[task-1].assignment_heap.push(task_assignment);
                tl.tasks_all[task-1].ta.insert(std::make_pair(agent_pos_pair, handle));
            }
        }
    }
}
