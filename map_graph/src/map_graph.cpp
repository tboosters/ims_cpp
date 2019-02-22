/*
 * Data structure of map graph. Stores graph information and provides routing and update operations.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cmath>

#include "../include/ims/map_graph.h"
#include "partition.h"

using namespace std;

/* Serialization */
void IMS::MapGraph::serialize(const string &output_file_path)
{
    ofstream ofs(output_file_path);
    boost::archive::text_oarchive output_archive_stream(ofs);
    output_archive_stream << *this;
    ofs.close();
}


/* Pre-processing */
void IMS::MapGraph::partition
        (const int &k, const int &l)
{
    vector<unsigned int> nodes(latitude.size());
    for(unsigned i = 0; i < nodes.size(); i++) nodes[i] = i;

    IMS::Partition::partition_t * partition = IMS::Partition::do_partition(nodes, latitude, longitude, k, l, 0);
    IMS::Partition::layer_t layer = IMS::Partition::build_layer(partition, latitude.size());
    IMS::Partition::print_layer(layer);
}

void IMS::MapGraph::preprocess()
{

}

/* Routing */
double IMS::MapGraph::find_current_density(unsigned edge, time_t enter_time)
{
    auto latest_density = lower_bound(current_density[edge].begin(), current_density[edge].end(), enter_time);
    return latest_density->second;
}

IMS::Path IMS::MapGraph::route(const double &origin_long, const double &origin_lat, const double &dest_long,
                               const double &dest_lat, const time_t &start_time)
{
    return IMS::Path();
}
