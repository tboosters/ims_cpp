/*
 * Header file for preprocess module.
 * Version: 1.0
 * Author: Yuen Hoi Man
 */

#ifndef IMS_CPP_PREPROCESS_H
#define IMS_CPP_PREPRECESS_H

#include <vector>
#include <ims/map_graph.h>
#include "partition.h"

using namespace std;

namespace IMS
{
namespace Preprocess
{

/* Stores an entry of a distane table
 * Fields: vector<unsigned> partition_distance: list of distances to each node within the same layer
 *         unsigned outbound_distance: distance towards the edge of the layer
 *         unsigned inbound_distance: distance from the edge of the layer
 */
struct entry_t
{
    vector<unsigned> partition_distance;
    unsigned outbound_distance;
    unsigned inbound_distance;
};
typedef struct entry_t entry_t;
typedef vector< vector< vector<entry_t>>> distance_table_t;

// Preprocessing
distance_table_t* do_preprocess (const vector<unsigned> &nodes, 
        const IMS::MapGraph* graph, IMS::Partition::partition_t* partitions, IMS::Partition::layer_t* layers);


}
}

#endif //IMS_CPP_PREPROCESS_H