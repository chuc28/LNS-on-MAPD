#include <vector>
#include <queue>
using std::vector;
using std::priority_queue;


struct Task
{
    int task_id;
    int priority;
    int pick_up_loc;
    int delivery_loc;
    int release_time;
    int delta_cost;
    float relatedness;
    int pick_up_time;
    int delivery_time;
    vector<TaskAssignment*> ta;
    priority_queue<TaskAssignment*, vector<TaskAssignment*>, CompareTaskAssignment> assignment_heap;
};

struct TaskAssignment
{
    int agent;
    int pos;
    int marginal_cost;
    TaskAssignment(int agent, int pos, int marginal_cost)
        : agent(agent), pos(pos), marginal_cost(marginal_cost)
        {}
};

struct CompareTaskAssignment {
    bool operator()(TaskAssignment* ta1, TaskAssignment* ta2)
    {
        return ta1->marginal_cost > ta2->marginal_cost;
    }
};

class TasksLoader{
public:
    vector<Task> tasks_all;
    static inline bool compareTask(Task& t1, Task& t2, bool insert, int insertion_strategy, int removal_strategy)
    {

        if (insert && insertion_strategy == 1) {
            return t1.assignment_heap.top()->marginal_cost <= t2.assignment_heap.top()->marginal_cost;
        }
        else if (insert && insertion_strategy == 2) {
            TaskAssignment* ta1 = t1.assignment_heap.top();
            int t1_first_best = ta1->marginal_cost;
            t1.assignment_heap.pop();
            int t1_second_best = t1.assignment_heap.top()->marginal_cost;
            t1.assignment_heap.push(ta1);
            TaskAssignment* ta2 = t1.assignment_heap.top();
            int t2_first_best = ta2->marginal_cost;
            t2.assignment_heap.pop();
            int t2_second_best = t2.assignment_heap.top()->marginal_cost;
            t1.assignment_heap.push(ta2);
            return t1_second_best - t1_first_best >= t2_second_best - t2_first_best;
        }
        if (removal_strategy == 1) {
            return t1.relatedness < t2.relatedness;
        }
        else if (removal_strategy == 2) {
            return t1.delta_cost < t2.delta_cost;
        }
    }


};