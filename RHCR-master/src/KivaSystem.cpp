#include "KivaSystem.h"
#include "WHCAStar.h"
#include "ECBS.h"
#include "LRAStar.h"
#include "PBS.h"

KivaSystem::KivaSystem(const KivaGrid& G, MAPFSolver& solver): BasicSystem(G, solver), G(G) {}

KivaSystem::~KivaSystem()
{
}

bool KivaSystem::load_task_assignments(string fname)
{
	string line;
	std::ifstream myfile((fname).c_str());
	if (!myfile.is_open()) {
		return false;
	}

	int id, start_loc, goal_loc, release_time;
	int seq_id = 0;
	task_sequences.clear();
	while (getline(myfile, line)) {
		std::istringstream iss(line);
		iss >> id;
		if (id <= G.agent_home_locations.size()) {
			task_sequences.emplace_back();
		}
		else {
			iss >> release_time >> start_loc >> goal_loc;
			task_sequences.back().emplace_back(G.endpoints[start_loc], release_time);
			task_sequences.back().emplace_back(G.endpoints[goal_loc], release_time);
		}
	}
	myfile.close();
	return true;
}

void KivaSystem::initialize()
{
	initialize_solvers();

	starts.resize(num_of_drives);
	goal_locations.resize(num_of_drives);
	paths.resize(num_of_drives);
	finished_tasks.resize(num_of_drives);
	bool succ = load_records(); // continue simulating from the records
	if (!succ)
	{
		timestep = 0;
		succ = load_locations();
		if (!succ)
		{
			// cout << "Randomly generating initial locations" << endl;
			initialize_start_locations();
			initialize_goal_locations();
		}
	}
}

void KivaSystem::initialize_start_locations()
{
	// Choose random start locations
	// Any non-obstacle locations can be start locations
	// Start locations should be unique
	for (int k = 0; k < num_of_drives; k++)
	{
		int orientation = -1;
		if (consider_rotation)
		{
			orientation = rand() % 4;
		}
		starts[k] = State(G.agent_home_locations[k], 0, orientation);
		paths[k].emplace_back(starts[k]);
		// finished_tasks[k].emplace_back(G.agent_home_locations[k], 0);
	}
}

void KivaSystem::initialize_goal_locations()
{
	if (hold_endpoints || useDummyPaths)
		return;
	// Choose random goal locations
	// Goal locations are not necessarily unique
	for (int k = 0; k < num_of_drives; k++)
	{
		// int goal = G.endpoints[rand() % (int)G.endpoints.size()];
		// goal_locations[k].emplace_back(goal, 0);
		if (task_sequences[k].empty()) {
			goal_locations[k].emplace_back(G.agent_home_locations[k], 0);
		}
		else {
			auto next = task_sequences[k].front();
			task_sequences[k].pop_front();
			goal_locations[k].emplace_back(next);
		}
	}
}

void KivaSystem::update_goal_locations()
{
	if (hold_endpoints)
	{
		unordered_map<int, int> held_locations; // <location, agent id>
		for (int k = 0; k < num_of_drives; k++)
		{
			int curr = paths[k][timestep].location; // current location
			if (goal_locations[k].empty())
			{
				int next = G.endpoints[rand() % (int)G.endpoints.size()];
				while (next == curr || held_endpoints.find(next) != held_endpoints.end())
				{
					next = G.endpoints[rand() % (int)G.endpoints.size()];
				}
				goal_locations[k].emplace_back(next, 0);
				held_endpoints.insert(next);
			}
			if (paths[k].back().location == goal_locations[k].back().first &&  // agent already has paths to its goal location
				paths[k].back().timestep >= goal_locations[k].back().second) // after its relase time
			{
				int agent = k;
				int loc = goal_locations[k].back().first;
				auto it = held_locations.find(loc);
				while (it != held_locations.end()) // its start location has been held by another agent
				{
					int removed_agent = it->second;
					if (goal_locations[removed_agent].back().first != loc)
						cout << "BUG" << endl;
					new_agents.remove(removed_agent); // another agent cannot move to its new goal location
					cout << "Agent " << removed_agent << " has to wait for agent " << agent << " because of location " << loc << endl;
					held_locations[loc] = agent; // this agent has to keep holding this location
					agent = removed_agent;
					loc = paths[agent][timestep].location; // another agent's start location
					it = held_locations.find(loc);
				}
				held_locations[loc] = agent;
			}
			else // agent does not have paths to its goal location yet
			{
				if (held_locations.find(goal_locations[k].back().first) == held_locations.end()) // if the goal location has not been held by other agents
				{
					held_locations[goal_locations[k].back().first] = k; // hold this goal location
					new_agents.emplace_back(k); // replan paths for this agent later
					continue;
				}
				// the goal location has already been held by other agents 
				// so this agent has to keep holding its start location instead
				int agent = k;
				int loc = curr;
				cout << "Agent " << agent << " has to wait for agent " << held_locations[goal_locations[k].back().first] << " because of location " <<
					goal_locations[k].back().first << endl;
				auto it = held_locations.find(loc);
				while (it != held_locations.end()) // its start location has been held by another agent
				{
					int removed_agent = it->second;
					if (goal_locations[removed_agent].back().first != loc)
						cout << "BUG" << endl;
					new_agents.remove(removed_agent); // another agent cannot move to its new goal location
					cout << "Agent " << removed_agent << " has to wait for agent " << agent << " because of location " << loc << endl;
					held_locations[loc] = agent; // this agent has to keep holding its start location
					agent = removed_agent;
					loc = paths[agent][timestep].location; // another agent's start location
					it = held_locations.find(loc);
				}
				held_locations[loc] = agent;// this agent has to keep holding its start location
			}
		}
	}
	else
	{
		for (int k = 0; k < num_of_drives; k++)
		{
			int curr = paths[k][timestep].location; // current location
			if (useDummyPaths)
			{
				if (goal_locations[k].empty())
				{
					goal_locations[k].emplace_back(G.agent_home_locations[k], 0);
				}
				if (goal_locations[k].size() == 1)
				{
					int next;
					do {
						next = G.endpoints[rand() % (int)G.endpoints.size()];
					} while (next == curr);
					goal_locations[k].emplace(goal_locations[k].begin(), next, 0);
					new_agents.emplace_back(k);
				}
			}
			else
			{
				pair<int, int> goal; // The last goal location
				if (goal_locations[k].empty())
				{
					goal = make_pair(curr, 0);
				}
				else
				{
					goal = goal_locations[k].back();
				}
				int min_timesteps = std::max(G.get_Manhattan_distance(goal.first, curr),
					goal.second - timestep); // G.heuristics.at(goal)[curr];
				while (min_timesteps <= simulation_window)
					// The agent might finish its tasks during the next planning horizon
				{
					// assign a new task
					pair<int, int> next;
					if (task_sequences[k].empty()) {
						break;
					}
					else {
						next = task_sequences[k].front();
						task_sequences[k].pop_front();
						goal_locations[k].emplace_back(next);
					}
					min_timesteps += G.get_Manhattan_distance(next.first, goal.first); // G.heuristics.at(next)[goal];
					min_timesteps = std::max(min_timesteps, goal.second - timestep);
					goal = next;
				}
				// if (task_sequences[k].empty()) {
				// 	cout << "Agent " << k << " has finished its task sequence" << 
				// 		" with path length " << paths[k].size() <<endl;
				// }
				if (goal_locations[k].empty())
				{
					// if (useDummyPaths) {
					int next = G.endpoints[rand() % (int)G.endpoints.size()];
					// 	} while (next == curr);
					goal_locations[k].emplace(goal_locations[k].begin(), next, 0);
					// 	new_agents.emplace_back(k);
					// }
					// else 
					// { // issue!""
						goal_locations[k].emplace_back(G.agent_home_locations[k], 0);
					// }
				}
			}
		}
	}
}

void KivaSystem::simulate(int simulation_time)
{
	std::clock_t c_start = std::clock();
	std::cout << "*** Simulating " << seed << " ***" << std::endl;
	this->simulation_time = simulation_time;
	initialize();
	vector<int> path_len;
	for (int i = 0; i < num_of_drives; i++) {
		path_len.push_back(0);
	}

	for (; timestep < simulation_time; timestep += simulation_window)
	{
		std::cout << "Timestep " << timestep << std::endl;

		update_start_locations();
		update_goal_locations();
		solve();

		// move drives
		auto new_finished_tasks = move();
		int old = num_of_tasks;

		// update tasks
		for (auto task : new_finished_tasks)
		{
			int id, loc, t;
			std::tie(id, loc, t) = task;
			finished_tasks[id].emplace_back(loc, t);
			num_of_tasks++;
			if (hold_endpoints)
				held_endpoints.erase(loc);
		}

		if (screen > 0)
		{
			std::cout << num_of_tasks - old << " tasks just finished" << std::endl;
			std::cout << num_of_tasks << " tasks finished in total" << std::endl;
			// std::cout << task_sequences.size() << " paths remain" << std::endl;
		}

		if (congested())
		{
			cout << "***** Too many traffic jams ***" << endl;
			break;
		}

		int k = 0;
		for (; k < num_of_drives; k++) {
			if (task_sequences[k].empty() && path_len[k] == 0) {
				path_len[k] = paths[k].size();
				cout << "Agent " << k << " has finished its task sequence" << 
					" with path length " << path_len[k] <<endl;
			}
		}
	}
	
	update_start_locations();
	std::clock_t c_end = std::clock();
	std::cout << std::endl << "Done! " << std::endl;
	int max = *std::max_element(path_len.begin(), path_len.end());
	cout<< "Makespan:  " << max << " Runtime: " << 1.0 * (c_end - c_start) / CLOCKS_PER_SEC << " s" << endl;
	save_results();
}

// int KivaSystem::get_makespan()
// {
// 	while (paths[0].size() > 1)
// 	{
// 		int T = (int)paths[0].size() - 1;
// 		for (const auto& path : paths)
// 		{
// 			if (find(G.agent_home_locations.begin(), G.agent_home_locations.end(), path[T - 1].location) != G.agent_home_locations.end())
// 			{
// 				return T;
// 			}
// 		}
// 		for (int i = 0; i < int(paths.size()); i++)
// 		{
// 			paths[i].pop_back();
// 		}
// 	}
// 	return -1; // should never happen
// }