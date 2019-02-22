/*
 * Header file for partition module.
 * Version: 1.0
 * Author: Terence Chow & Calvin Yuen
 */

#ifndef IMS_CPP_PARTITION_H
#define IMS_CPP_PARTITION_H

#include <vector>
#include <unordered_map>
#include <ims/map_graph.h>

using namespace std;

namespace IMS
{
namespace Partition
{

/* Stores a partition:
 * Fields: unsigned id: partition / node id
 *         bool is_node
 *         vector<unsigned> boundary_nodes
 *         vector<partition_t*> sub_partition: list of pointers to all children
 */
struct partition_t
{
    unsigned id;
    bool is_node;
    vector<unsigned> boundary_nodes;
    vector<partition_t*> sub_partition;
};
typedef struct partition_t partition_t;


/* Records parent of each node / partition in the partition hierarchy
 * e.g. layer[x][y] = Partition ID of parent of a node / partition y in level x
 */
typedef vector< vector<unsigned> > layer_t;


/* Partition Scheme */
unordered_map<long, vector<unsigned> > grid_partition
        (const vector<unsigned> &nodes, const IMS::MapGraph * graph, const int &k);

/* Partitioning & Layering */
partition_t * do_partition
            (const vector<unsigned> &nodes, const IMS::MapGraph *graph,
             const int &k, const int &l, const unsigned &partition_id);

void index_partition(partition_t * p);

layer_t build_layer(IMS::Partition::partition_t * p, const unsigned long & num_of_nodes);

/* Util functions */
void print_partition(partition_t * p);

void print_layer(const layer_t & layer);

long find_parent(const layer_t &layer, const long &node, const long &level = -1);

void delete_partition(partition_t *& p);

}
}


#endif //IMS_CPP_PARTITION_H
