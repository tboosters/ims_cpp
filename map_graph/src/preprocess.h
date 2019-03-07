/*
 * Header file for preprocess module.
 * Version: 1.0
 * Author: Yuen Hoi Man
 */

#ifndef IMS_CPP_PREPROCESS_H
#define IMS_CPP_PREPROCESS_H

#include <boost/serialization/map.hpp>

#include <vector>
#include "partition.h"

using namespace std;

namespace IMS
{
namespace Preprocess
{

/* Stores an entry of a distane table
 * Fields: map<unsigned, unsigned> partition_distance: list of distances to each node within the same layer. 
 *                                           e.g. partition_distance[n] = distane towards n
 *         unsigned outbound_distance: distance towards the edge of the layer
 *         unsigned inbound_distance: distance from the edge of the layer
 */
struct entry_t
{
    map<unsigned, unsigned> partition_distance;
    unsigned outbound_distance;
    unsigned inbound_distance;

    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & partition_distance;
        archive & outbound_distance;
        archive & inbound_distance;
    }
};
typedef struct entry_t entry_t;

/* Records the entirety of the distance table
 * e.g. distance_table[x][y] = distance information of node / partition y in level x
 * e.g. distance_table[x][y].partition_distance[z] = precomputed distane of node / partition y to z in level x
 */
typedef vector< vector< entry_t>> distance_table_t;

/* Preprocessing */
distance_table_t* do_preprocess 
        (const vector<unsigned> &nodes, 
         const vector<unsigned>& head,
         const vector<unsigned>& first_out,
         const vector<unsigned>& default_travel_time,
         IMS::Partition::partition_t* partitions, 
         IMS::Partition::layer_t* layers);

/* Util functions */
void print_distance_table (distance_table_t * distance_table);


}
}

#endif //IMS_CPP_PREPROCESS_H