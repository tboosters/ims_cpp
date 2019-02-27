/*
 * Data structure of map graph. Stores graph information and provides routing and update operations.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow & Yuen Hoi Man
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <routingkit/geo_position_to_node.h>

#include "../include/ims/map_graph.h"
#include "partition.h"

using namespace std;

/* Initialize dynamic fields: current_density, inversed
 * MUST BE CALLED AFTER CREATING CLASS INSTANCE.
 * Parameter: NIL
 * Return: when the fields are initialized
 * */
void IMS::MapGraph::initialize()
{
    for (unsigned i = 0; i < default_travel_time.size(); i++)
    {
        current_density.emplace_back();
        current_density[i][0] = 0;
    }

    inversed = inverse();

    map_geo_position = RoutingKit::GeoPositionToNode(latitude, longitude);
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


/** Creates an inversed MapGraph for the current graph that contains edges pointing to the opposite side
 * Parameter: NIL
 * Return: InversedGraph: inversed
 */
IMS::InversedGraph* IMS::MapGraph::inverse()
{
    auto inverse = new InversedGraph();

    // recalcuate inversed edges
    vector <vector <pair <unsigned, unsigned>>> temp_edges;
    for (unsigned int node = 0; node < this->first_out.size(); node ++)
    {
        vector< pair <unsigned, unsigned>> new_edge_container;
        temp_edges.push_back(new_edge_container);
    }
    for (unsigned int current_node = 0; current_node < this->first_out.size(); current_node++)
    {
        unsigned first_edge = this->first_out[current_node];
        unsigned last_edge = (current_node == this->first_out.size() -1) ? (this->head.size()) : this->first_out[current_node + 1];
        for (unsigned int current_edge = first_edge; current_edge < last_edge; current_edge ++)
        {
            temp_edges[this->head[current_edge]].push_back(pair <unsigned, unsigned> (current_edge, current_node));
        }
    }

    // translate edges into MapGraph data structure
    inverse->head.reserve(this->head.size());
    inverse->first_out.reserve(this->first_out.size());
    unsigned current_head_position = 0;
    for (unsigned int new_origin = 0; new_origin < temp_edges.size(); new_origin ++)
    {
        inverse->first_out.push_back(current_head_position);
        for (unsigned int new_edge = 0; new_edge < temp_edges[new_origin].size(); new_edge ++)
        {
            inverse->head.push_back(temp_edges[new_origin][new_edge].second);
            inverse->relative_edge[current_head_position] = temp_edges[new_origin][new_edge].first;
            current_head_position ++;
        }
    }
    return inverse;
}


/* Pre-processing */

/* Entrance function of partitioning of this MapGraph
 * Parameters: const int & k: number of partitions
 *             const int & l: number of levels
 * Return: when partitioning is done
 */
void IMS::MapGraph::partition(const int &k, const int &l)
{
    vector<unsigned int> nodes(latitude.size());
    for(unsigned i = 0; i < nodes.size(); i++) nodes[i] = i;

    IMS::Partition::partition_t * partition = IMS::Partition::do_partition(nodes, this, k, l, 0);
    IMS::Partition::index_partition(partition);
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

/* Incident Managemet */

/* Determine if point q is in range / linear with line formed by points p1 and p2
 * Parameters: const float & p1_x
 *             const float & p1_y
 *             const float & p2_x
 *             const float & p2_y
 *             const float & q_x
 *             const float & q_y
 *             const float & offset: offset applied to x and y axis to tolerate q's coordinate
 * Returns: bool: whether q is within the range of the line p1p2
 */
bool is_in_line_range(const float &p1_x, const float &p1_y, const float &p2_x, const float &p2_y, const float &q_x,
                      const float &q_y, const float &offset)
{
    if(q_x >= min(p1_x, p2_x)-offset && q_x <= max(p1_x, p2_x)+offset
       && q_y >= min(p1_y, p2_y)-offset && q_y <= max(p1_y, p2_y)+offset)
    {
        if(p2_x == p1_x || p2_y == p1_y)
        {
            // vertical line or horizontal line
            return true;
        }

        // y = mx + c
        double m = (p2_y - p1_y) / (p2_x - p1_x);
        double c = m * (-p1_x) + p1_y;

        if(m*q_x+c >= q_y-offset && m*q_x+c <= q_y+offset)
        {
           return true;
        }
        if((q_y-c)/m >= q_x-offset && (q_y-c)/m <= q_x+offset)
        {
            return true;
        }
    }
    return false;
}

/* Find which edge this location is on or the nearest edge.
 * Parameters: const float & lat
 *             const float & longi
 *             const float & offset: offset applied to x and y axis to tolerate q's coordinate
 * Returns: unsigned: nearest edge id, INFINITY if not found
 */
unsigned IMS::MapGraph::find_nearest_edge_of_location(const float &lat, const float &longi, const float &offset)
{
    for(unsigned tail = 0; tail < first_out.size(); tail++)
    {
        for(unsigned edge = first_out[tail]; edge < first_out[tail+1] || (tail+1 == first_out.size() && edge < head.size()); edge++)
        {
            if(is_in_line_range(longitude[tail], latitude[tail], longitude[head[edge]], latitude[head[edge]], longi, lat, offset))
            {
                return edge;
            }
        }
    }

    return INFINITY;
}

/* Utils */

/* Print graph structure
 * Paramters: NIL
 * Returns: when finish printing graph.
 */
void IMS::MapGraph::print_graph()
{
    for (unsigned node = 0; node < this->first_out.size(); node ++)
    {
        cout << "[" << node << "] -> ";
        unsigned first_edge = this->first_out[node];
        unsigned last_edge = (node == this->first_out.size() -1) ? (this->head.size()) : this->first_out[node + 1];
        for (unsigned edge = first_edge; edge < last_edge; edge ++)
        {
            cout << this->head[edge] << ", ";
        }
        cout << endl;
    }
}

/* Print inversed graph structure
 * Paramters: NIL
 * Returns: when finish printing graph.
 */
void IMS::MapGraph::print_inversed_graph()
{
    for (unsigned node = 0; node < this->inversed->first_out.size(); node ++)
    {
        cout << "[" << node << "] -> ";
        unsigned first_edge = this->inversed->first_out[node];
        unsigned last_edge = (node == this->inversed->first_out.size() -1) ? (this->inversed->head.size()) : this->inversed->first_out[node + 1];
        for (unsigned edge = first_edge; edge < last_edge; edge ++)
        {
            cout << this->inversed->head[edge] << ", ";
        }
        cout << endl;
    }
}