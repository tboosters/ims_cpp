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
#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "../include/ims/map_graph.h"
//#include "partition.h"
//#include "preprocess.h"

using namespace std;

/* Destructor for releasing dynamic memory allocated to MapGraph.
 * Parameter(s): NIL
 * Return: when memory is released.
 */
IMS::MapGraph::~MapGraph()
{
    delete inversed;
    delete layers;
    delete distance_tables;
}

/* Initialize dynamic fields: current_density, inversed, map_geo_location
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

/* Entrance function of preprocessing of this MapGraph
 * Parameters: const int & k: number of partitions
 *             const int & l: number of levels
 * Return: when preprocess is done
 */
void IMS::MapGraph::preprocess(const unsigned &k, const unsigned &l)
{
    // Prepare node information
    vector<unsigned int> nodes(latitude.size());
    for(unsigned i = 0; i < nodes.size(); i++) nodes[i] = i;

    // Partition
    IMS::Partition::partition_t* partitions = IMS::Partition::do_partition(
            nodes, this->latitude , this->longitude,
            this->head, this->first_out, this->inversed->head, this->inversed->first_out,
            k, l, 0);
    IMS::Partition::index_partition(partitions);
    IMS::Partition::layer_t* layers = IMS::Partition::build_layer(partitions, latitude.size());

    // Preprocessing
    auto distance_tables = IMS::Preprocess::do_preprocess(
            nodes,
            this->head, this->first_out, this->default_travel_time,
            partitions, layers);
    
    // save information
    this->layers = layers;
    this->distance_tables = distance_tables;

    // Release memory
    delete partitions;
}

/* Routing */

/* Find the edge id from one node to another.
 * Paramter: unsigned &from: from edge
 *           unsigned &to: to edge
 * Return: unsigned: edge id, INFINITY if no edge exist
 */
unsigned IMS::MapGraph::find_edge(const unsigned &from, const unsigned &to)
{
    unsigned first_edge = first_out[from];
    unsigned last_edge = (from == first_out.size() -1) ? (head.size()) : first_out[from + 1];
    for (unsigned int current_edge = first_edge; current_edge < last_edge; current_edge ++)
    {
        if (head[current_edge] == to)
        {
            return current_edge;
        }
    }
    return INFINITY;
}

/* Find latest effective density: that with time less than or equal to the enter time.
 * Parameters: unsigned edge: edge ID
 *            time_t enter_time
 * Return: double: latest effective density
 * */
double IMS::MapGraph::find_current_density(unsigned edge, time_t enter_time)
{
    // Lock reader access
    boost::shared_lock<boost::shared_mutex> reader_lock(access);

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

/* Updating */

/* Inject the increased density information to each edge involved in the specified path.
 * Done according to when the vehicle enters and leaves the edge.
 * Parameter(s): IMS::Path * path
 * Returns: when update is done.
 */
void IMS::MapGraph::inject_impact_of_routed_path(IMS::Path *path)
{
    // Lock exclusive writer access
    boost::unique_lock<boost::shared_mutex> writer_lock(access);

    unsigned edge;
    time_t enter_time, leave_time;
    double density_delta;
    auto next_enter_time_edge = path->enter_times.begin();
    next_enter_time_edge++;

    for (auto &enter_time_edge : path->enter_times)
    {
        edge = enter_time_edge.second;
        enter_time = enter_time_edge.first;
        leave_time = next_enter_time_edge == path->enter_times.end()? path->end_time : next_enter_time_edge->first;
        density_delta = 1.0 / geo_distance[edge];

        // When vehicle leaves edge, restore density
        // Case where leave_time exists can be skipped as traffic density will be retained as the same anyways
        auto before_leave_time = current_density[edge].lower_bound(leave_time);
        if(before_leave_time->first != leave_time)
        {
            current_density[edge][leave_time] = (--before_leave_time)->second;
        }

        // When vehicle enters edge
        auto before_enter_time = current_density[edge].lower_bound(enter_time);
        if(before_enter_time->first == enter_time)
        {
            current_density[edge][enter_time] += density_delta;
        }
        else
        {
            current_density[edge][enter_time] = (--before_enter_time)->second + density_delta;
        }

        // When vehicle is in the edge
        for(auto intermediate = ++(current_density[edge].find(enter_time)); intermediate->first != leave_time; intermediate++)
        {
            intermediate->second += density_delta;
        }

        next_enter_time_edge++;
    }
}

/* Remove the impact on density brought by the specified path from each edge involved.
 * Parameter(s): IMS::Path * path
 * Returns: when update is done.
 */
void IMS::MapGraph::remove_impact_of_routed_path(IMS::Path *path)
{
    // Lock exclusive writer access
    boost::unique_lock<boost::shared_mutex> writer_lock(access);

    unsigned edge;
    time_t enter_time, leave_time;
    double density_delta;
    auto next_enter_time_edge = path->enter_times.begin();
    next_enter_time_edge++;

    for (auto &enter_time_edge : path->enter_times)
    {
        edge = enter_time_edge.second;
        enter_time = enter_time_edge.first;
        leave_time = next_enter_time_edge == path->enter_times.end()? path->end_time : next_enter_time_edge->first;
        density_delta = 1.0 / geo_distance[edge];

        // When vehicle enters edge
        current_density[edge][enter_time] -= density_delta;

        // When vehicle is in the edge
        for(auto intermediate = ++(current_density[edge].find(enter_time)); intermediate->first != leave_time; intermediate++)
        {
            intermediate->second -= density_delta;
        }

        next_enter_time_edge++;
    }
}

/* Reverse Geocoding */

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
 * Returns: vector<unsigned>: nearest edges' id, empty if not found
 */
vector<unsigned> IMS::MapGraph::find_nearest_edge_of_location(const float &longi, const float &lat,
                                                                  const float &offset)
{
    vector<unsigned> nearest_edges;
    for(unsigned tail = 0; tail < first_out.size(); tail++)
    {
        for(unsigned edge = first_out[tail];
            (tail+1 < first_out.size() && edge < first_out[tail+1]) || (tail+1 == first_out.size() && edge < head.size());
            edge++)
        {
            if(is_in_line_range(longitude[tail], latitude[tail], longitude[head[edge]], latitude[head[edge]], longi, lat, offset))
            {
                nearest_edges.push_back(edge);
            }
        }
    }

    return nearest_edges;
}

/* Entrance function to find which node this location is on or the nearest node.
 * Parameters: const float & lat
 *             const float & longi
 *             const float & radius
 * Returns: unsigned: nearest nodes' id, RoutingKit::invalid_id if not found
 */
unsigned IMS::MapGraph::find_nearest_node_of_location(const float &longi, const float &lat, const float & radius)
{
    return map_geo_position.find_nearest_neighbor_within_radius(lat, longi, radius).id;
}

/**/

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