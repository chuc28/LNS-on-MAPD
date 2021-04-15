#pragma once
#include <vector>
#include <queue>
#include <map>

using std::vector;
using std::priority_queue;
using std::size_t;
using std::string;
using std::hash;
typedef std::pair<int, int> Key;

struct TaskAssignment
{
    int agent;
    int pos;
    int insertion_cost;
    TaskAssignment(int agent, int pos, int insertion_cost)
        : agent(agent), pos(pos), insertion_cost(insertion_cost)
        {};
};

struct CompareTaskAssignment {
    bool operator()(TaskAssignment* ta1, TaskAssignment* ta2)
    {
        return ta1->insertion_cost > ta2->insertion_cost;
    }
};

class Task{
public:
    int task_id;
    int priority;
    int pick_up_loc;
    int delivery_loc;
    int release_time;
    int delta_cost;
    float relatedness;
    int pick_up_time;
    int delivery_time;
    std::map<Key, TaskAssignment*> ta; 
    priority_queue<TaskAssignment*, vector<TaskAssignment*>, CompareTaskAssignment> assignment_heap;
    
    int agent_id;
    unsigned int ag_arrive_start;
    unsigned int ag_arrive_goal;
    bool isdelivered;
    
    Task();
    Task(int Id, int release_time, int pick_up_loc, int delivery_loc, int pick_up_time, int delivery_time)
        :task_id(Id), release_time(release_time), pick_up_loc(pick_up_loc), delivery_loc(delivery_loc), pick_up_time(pick_up_time), delivery_time(delivery_time), isdelivered(false) 
        {};
    // ~Task();
};

class TasksLoader{
public:
    int num_of_tasks;
    vector<Task> tasks_all;
    static inline bool compareTask(Task& t1, Task& t2, bool insert, int insertion_strategy, int removal_strategy)
    {
        if (insert && insertion_strategy == 1) {
            return t1.assignment_heap.top()->insertion_cost <= t2.assignment_heap.top()->insertion_cost;
        }
        else if (insert && insertion_strategy == 2) {
            TaskAssignment* ta1 = t1.assignment_heap.top();
            int t1_first_best = ta1->insertion_cost;
            t1.assignment_heap.pop();
            int t1_second_best = t1.assignment_heap.top()->insertion_cost;
            t1.assignment_heap.push(ta1);
            TaskAssignment* ta2 = t2.assignment_heap.top();
            int t2_first_best = ta2->insertion_cost;
            t2.assignment_heap.pop();
            int t2_second_best = t2.assignment_heap.top()->insertion_cost;
            t1.assignment_heap.push(ta2);
            return t1_second_best - t1_first_best >= t2_second_best - t2_first_best;
        }
        else if (removal_strategy == 1) {
            return t1.relatedness < t2.relatedness;
        }
        else if (removal_strategy == 2) {
            return t1.delta_cost < t2.delta_cost;
        }
        return true;
    }
    TasksLoader(const std::string fname);
    TasksLoader();
    // ~TasksLoader();
};
