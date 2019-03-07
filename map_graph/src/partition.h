/*
 * Header file for partition module.
 * Version: 1.0
 * Author: Terence Chow & Yuen Hoi Man
 */

#ifndef IMS_CPP_PARTITION_H
#define IMS_CPP_PARTITION_H

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <vector>
#include <unordered_map>

using namespace std;

namespace IMS
{
namespace Partition
{

/* Stores a partition:
 * Fields: unsigned id: partition / node id
 *         bool is_node
 *         vector<unsigned> boundary_outwards: list of nodes with outward edges
 *         vector<unsigned> boundary_inwards: list of nodes with inward edges
 *         partition_t* parent partition: pointer to parent
 *         vector<partition_t*> sub_partition: list of pointers to all children
 */
struct partition_t
{
    unsigned id;
    unsigned layer;
    bool is_node;
    vector<unsigned> boundary_outwards;
    vector<unsigned> boundary_inwards;
    partition_t* parent_partition;
    vector<partition_t*> sub_partition;

    ~partition_t()
    {
        for(auto sp : sub_partition)
        {
            delete sp;
        }
    }
};
typedef struct partition_t partition_t;


/* Records parent of each node / partition in the partition hierarchy
 * e.g. layer[x][y] = Partition ID of parent of a node / partition y in level x
 */
typedef vector< vector<unsigned> > layer_t;


/* Partition Scheme */
unordered_map<long, vector<unsigned> > grid_partition
        (const vector<unsigned> &nodes, 
         const vector<float> &latitude, 
         const vector<float> &longitude, 
         const int &k);

/* Partitioning & Layering */
partition_t * do_partition
        (const vector<unsigned> &nodes, 
         const vector<float> &latitude, 
         const vector<float> &longitude,
         const vector<unsigned> &head, 
         const vector<unsigned> &first_out, 
         const vector<unsigned> &head_inversed, 
         const vector<unsigned> &first_out_inversed, 
         const int &k, 
         const int &l,
         const unsigned &partition_id,
        const unsigned layer = 0);

void index_partition(partition_t * p);

layer_t* build_layer(IMS::Partition::partition_t * p, const unsigned long & num_of_nodes);

/* Util functions */
void print_partition(partition_t * p);

void print_layer(const layer_t* layer);

long find_parent(const layer_t* layer, const long &node, const long &level = -1);

void delete_partition(partition_t *& p);

}
}


#endif //IMS_CPP_PARTITION_H
