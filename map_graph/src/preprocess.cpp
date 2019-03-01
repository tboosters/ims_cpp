/*
 * Preprocess functions. All functions are free functions in IMS::Routing namespace.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow
 */

#include <iostream>
//#include <cmath>

#include "preprocess.h"

using namespace std;
using namespace IMS::Preprocess;

distance_table_t* IMS::Preprocess::do_preprocess (const vector<unsigned> &nodes, 
        const IMS::MapGraph* graph, IMS::Partition::partition_t* partitions, IMS::Partition::layer_t* layers)
{        
    distance_table_t* distance_table = new distance_table_t;
    return distance_table;
}
