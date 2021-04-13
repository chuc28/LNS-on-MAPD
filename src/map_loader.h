/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/

#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <string>
#include <vector>
#include "agents_loader.h"

using namespace std;

#define TIME_LIMIT 300000

struct Endpoint{
    int loc;
    int id;
};

class MapLoader{
public:
    bool* my_map;
    Endpoint * endpoint;
    Agent * agent;
    int rows;
    int cols;
    int endpoint_num;
    int agent_num;
    int maxtime;
    int* moves_offset;

    enum valid_moves_t { UP, RIGHT, DOWN, LEFT, WAIT_MOVE, MOVE_COUNT };  // MOVE_COUNT is the enum's size

    MapLoader();
    MapLoader(std::string fname); // load map from file
    ~MapLoader();
    
    // void printMap ();
    // void printMap (char* mapChar);
    // char* mapToChar();
    inline bool is_blocked (int row, int col) const { return my_map[row * this->cols + col]; }
    inline bool is_blocked (int loc) const { return my_map[loc]; }
    inline size_t map_size() const { return rows * cols; }
    inline int row_coordinate(int id) const { return id / this->cols; }
    inline int col_coordinate(int id) const { return id % this->cols; }
};
#endif
