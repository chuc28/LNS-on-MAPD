/*
 * This code is modified from https://github.com/Jiaoyang-Li/Flatland
*/

#include "map_loader.h"
#include <string>
#include <cstring>
#include <iostream>
#include <cassert>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <climits>
#include <float.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace boost;
using namespace std;

MapLoader::MapLoader(){}

MapLoader::MapLoader(string fname){
    string line;
    ifstream myfile(fname.c_str());
    if (myfile.is_open())
    {
        getline (myfile,line);
        char_separator<char> sep(",");
        tokenizer< char_separator<char> > tok(line, sep);
        tokenizer< char_separator<char> >::iterator beg = tok.begin();
        int rows = atoi ((*beg).c_str()); // read number of rows
        this->rows = rows;
        beg++;
        int cols = atoi ((*beg).c_str()); // read number of cols
        this->cols = cols;

        stringstream ss;
        int endpoint_num;
        getline(myfile, line);
        ss << line;
        ss >> endpoint_num;
        this->endpoint_num = endpoint_num;

        int agent_num;
        ss.clear();
        getline(myfile, line);
        ss << line;
        ss >> agent_num;
        this->agent_num = agent_num;

        int maxtime;
        ss.clear();
        getline(myfile, line);
        ss << line;
        ss >> maxtime;
        this->maxtime = maxtime;
        
        bool* my_map= new bool[rows*cols];
        Endpoint* endpoint = new Endpoint[endpoint_num+agent_num];
        Agent* agent = new Agent[agent_num];
        // for (int i=0; i<rows*cols; i++)
        //     my_map[i] = false;
        // read map (and start/goal locations)
        int m = 0, n = 0;
        for (int i = 1; i<rows - 1; i++)
        {
            getline(myfile, line);
            for (int j = 1; j<col - 1; j++)
            {
                my_map[cols*i + j] = (line[j - 1] != '@'); // not a block
                if (line[j - 1] == 'e') //endpoint
                {
                    endpoints[m++].loc = i*cols + j;
                }
                else if (line[j - 1] == 'r') //robot start location
                {
                    endpoints[endpoint_num + n].loc = i*cols + j;
                    agents[n].Set(i*cols + j, col, rows, n, maxtime);
                    n++;
                }
            }
        }
        for (int i=0; i<rows; i++)
        {
            getline (myfile, line);
            for (int j=0; j<cols; j++)
            {
                my_map[cols*i + j] = (line[j] != '.');
            }
        }

        // add padding
        for (int i = 0; i < row; i++)
        {
            my_map[i*col] = false;
            my_map[i*col + col - 1] = false;
        }
        for (int j = 1; j < col - 1; j++)
        {
            my_map[j] = false;
            my_map[row*col - col + j] = false;
        }
        this->my_map = my_map;

        // initialize moves_offset array
        moves_offset = new int[5];
        moves_offset = new int[MapLoader::MOVE_COUNT];
        moves_offset[MapLoader::valid_moves_t::WAIT_MOVE] = 0;
        moves_offset[MapLoader::valid_moves_t::UP] = -cols;
        moves_offset[MapLoader::valid_moves_t::RIGHT] = 1;
        moves_offset[MapLoader::valid_moves_t::DOWN] = cols;
        moves_offset[MapLoader::valid_moves_t::LEFT] = -1;
    }
    else
    {
        cerr << "Map file" << fname << "not found." << endl;
        return;
    }
    myfile.close();
}

MapLoader::~MapLoader() {
  delete[] this->my_map;
  delete[] this->moves_offset;
}

// char* MapLoader::mapToChar() {
//     char* mapChar = new char[rows*cols];
//     for (int i=0; i<rows*cols; i++) {
//         if ( i == start_loc )
//             mapChar[i] = 'S';
//         else if ( i == goal_loc )
//             mapChar[i] = 'G';
//         else if (this->my_map[i] == true)
//             mapChar[i] = '*';
//         else
//             mapChar[i] = ' ';
//     }
//     return mapChar;
// }

// void MapLoader::printMap () {
//     char* mapChar = mapToChar();
//     printMap (mapChar);
//     delete[] mapChar;
// }

// void MapLoader::printMap (char* mapChar) {
//     cout << "MAP:";
//     for (int i=0; i<rows*cols; i++) {
//         if (i % cols == 0)
//             cout << endl;
//         cout << mapChar[i];
//     }
//     cout << endl;
}





