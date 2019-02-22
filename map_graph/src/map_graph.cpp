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

/* Initialize dynamic fields: default_speed, current_density
 * Parameter: NIL
 * Return: when the fields are initialized
 * */
void IMS::MapGraph::initialize()
{
    for(unsigned i = 0; i < default_travel_time.size(); i++)
    {
        default_speed.push_back(geo_distance[i] / default_travel_time[i]);
    }

    for (unsigned i = 0; i < default_travel_time.size(); i++)
    {
        current_density.emplace_back();
        current_density[i][0] = 0;
    }
}

/** Serialize MapGraph information into persistent file
 * Parameter: const string & output_file_path
 * Return: when file is serialized
 */
void IMS::MapGraph::serialize(const string &output_file_path)
{
    ofstream ofs(output_file_path);
    boost::archive::text_oarchive output_archive_stream(ofs);
    output_archive_stream << *this;
    ofs.close();
}


/* Pre-processing */

/* Entrance function of partitioning of this MapGraph
 * Parameters: const int & k: number of partitions
 *             const int & l: number of levels
 * Return: when partitioning is done
 */
void IMS::MapGraph::partition
        (const int &k, const int &l)
{
    vector<unsigned int> nodes(latitude.size());
    for(unsigned i = 0; i < nodes.size(); i++) nodes[i] = i;

    IMS::Partition::partition_t * partition = IMS::Partition::do_partition(nodes, this, k, l, 0);
    IMS::Partition::layer_t layer = IMS::Partition::build_layer(partition, latitude.size());
    IMS::Partition::print_layer(layer);
}

/* Entrance function of preprocessing of this MapGraph
 * Parameters:
 * Return:
 */
void IMS::MapGraph::preprocess()
{

}

/* Routing */

/* Find latest effective density: that with time less than or equal to the enter time.
 * Parameters: unsigned edge: edge ID
 *            time_t enter_time
 * Return: double: latest effective density
 * */
double IMS::MapGraph::find_current_density(unsigned edge, time_t enter_time)
{
    auto latest_density = current_density[edge].lower_bound(enter_time);
    if(latest_density->first == enter_time)
    {
        return latest_density->second;
    }
    else
    {
        return (--latest_density)->second;
    }
}

/* Performs routing OD requests with this MapGraph starting at time and location specified.
 * Parameters: const double & origin_long
 *             const double & origin_lat
 *             const double & dest_long
 *             const double & dest_lat
 *             const time_t & start_time
 * Return: IMS::Path: Path found consisting all edges involved and enter time to each edge
 */
IMS::Path IMS::MapGraph::route(const double &origin_long, const double &origin_lat, const double &dest_long,
                               const double &dest_lat, const time_t &start_time)
{
    return IMS::Path();
}
