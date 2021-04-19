#include <vector>
#include <utility>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <fstream>
#include "tasks_loader.h"
using namespace std;

TasksLoader::TasksLoader(){
    num_of_tasks = 0;
}

TasksLoader::TasksLoader(string task_fname, string map_fname){  
    AgentsLoader* al = new AgentsLoader(map_fname);
    string line;
    ifstream myfile(task_fname.c_str());

    std::size_t p  = task_fname.find('-');
    std::size_t pp  = task_fname.find("task");
    this->frequency= task_fname.substr(p+1, pp-p-2); 
    
    if (myfile.is_open()){
        stringstream ss;
        int task_num;
        getline(myfile, line);
        ss << line;
        ss >> task_num;  // number of tasks
        this->num_of_tasks = task_num;
        for (int i = 0; i < task_num; i++)
        {
            int release_time, pick_up_loc, delivery_loc, pick_up_time, delivery_time;
            getline(myfile, line);
            ss.clear();
            ss << line;
            ss >> release_time >> pick_up_loc >> delivery_loc >> pick_up_time >> delivery_time;  // task release time, start, goal, pick up time, time at goal
            // pick_up_loc = al->endpoints[pick_up_loc].loc;
            // delivery_loc = al->endpoints[delivery_loc].loc;
            tasks_all.push_back(Task(i+1, release_time, pick_up_loc, delivery_loc, pick_up_time, delivery_time));
        }
    }
    else{
        cerr << "Task file " << task_fname << " not found." << endl;
        return;
    }
    myfile.close();
}

// TasksLoader::~TasksLoader() {}
