/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/

#pragma once

#include "agents_loader.h"
#include "map_loader.h"
#include "tasks_loader.h"
#include <chrono>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fstream>
using std::vector;
using std::cout;
using std::endl;
using namespace std::chrono;
using std::string;
typedef std::chrono::high_resolution_clock Time;

class LNS
{
public:
    int flowtime = 0;
    int makespan = 0;
    int initial_flowtime = 0;
    int initial_makespan = 0;
    int runtime = 0;
    float initial_runtime = 0;
    vector<int> neighbors;

    LNS(TasksLoader& tl, AgentsLoader& al, //MapLoader& ml, 
        int insertion_strategy, 
        int removal_strategy, int neighborhood_size, string outfile):
            tl(tl), al(al), ml(ml), insertion_strategy(insertion_strategy), 
            removal_strategy(removal_strategy),
            neighborhood_size(neighborhood_size){}
    bool run(float time_limit, int max_iterations);
    bool getInitialSolution();

private:
    high_resolution_clock::time_point start_time;
    TasksLoader& tl;
    AgentsLoader& al;
    MapLoader& ml;
    string outfile;

    int removal_strategy = 0; // 0: random; 1: shaw; 2: worst
    int insertion_strategy = 0; // 0: random; 1: basic_greedy; 2: regret
    int neighborhood_size = 0;
    int updated_agent = 0;
    int removed_task = 0;
    float relatedness_weight1 = 0;
    float relatedness_weight2 = 0;
    std::unordered_map<int, vector<int>> best_task_sequence;
    std::unordered_map<Key, TaskAssignment*>::iterator iter;

    void initializeAssignmentHeap();
    void sortNeighborsByStrategy();
    void addTaskAssignment();
    void updateAssignmentHeap();

    void generateNeighborsByShawRemoval();
    void generateNeighborsByWorstRemoval();
    void updateTaskSequenceAfterRemoval(vector<int> neighbors);

    // tool;
    int calculateMakespan(Agent agent, vector<int> task_sequence);
    int calculateFlowtime(Agent agent, vector<int> task_sequence);
    // int calculateMarginalCost(Task task, vector<int> task_sequence, int pos, Agent Agent);
    int calculateRegretValue(Task task, vector<int> task_sequence, int pos, Agent Agent);
    int calculateManhattanDistance(int loc1, int loc2);
    void quickSort(vector<int>& task_order, int low, int high, bool insert);
};