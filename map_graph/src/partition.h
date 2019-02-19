/*
 * Header file for partition module.
 * Version: 1.0
 * Author: Terence Chow
 */

#ifndef IMS_CPP_PARTITION_H
#define IMS_CPP_PARTITION_H

#include <vector>
#include <unordered_map>

using namespace std;

namespace IMS
{
namespace Partition
{

struct partition_t
{
    long partition_id;
    long node_id;
    vector<partition_t*> sub_partition;
};
typedef struct partition_t partition_t;

typedef vector< vector<long> > layer_t;


/* Partition Scheme */
unordered_map<long, vector<unsigned int> > grid_partition
        (const vector<unsigned> &nodes, const vector<float> &latitude,
                const vector<float> &longitude, const int &k);

/* Partitioning & Layering */
partition_t * do_partition
        (const vector<unsigned int> &nodes, const vector<float> & latitude, const vector<float> & longitude,
                const int & k, const int & l, const long & partition_id);

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
