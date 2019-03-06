/*
 * Routing functions. All functions are free functions in IMS::Routing namespace.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow @ Yuen Hoi Man
 */

#include <cmath>
#include <set>
#include <stack>
#include <vector>
#include <queue>

#include "../include/ims/router.h"

const unsigned INF  = ((int)INFINITY);

/* Future weight retrieval function, calculate the estimated future weight (heuristics) between two nodes.
 * Parameters: const unsigned from_node
 *             const unsigned to_node
 * Return: h(u, t) -> estimated time needed to travel from u to t
 */
unsigned IMS::Router::retrieve_future_weight(const unsigned &from_node, const unsigned &to_node)
{
    IMS::Preprocess::distance_table_t* distance_table = map_graph->distance_tables;
    IMS::Partition::layer_t* layers = map_graph->layers;
    unsigned future_weight = 0;
    // retrieve first partition
    unsigned from_partition = (*layers)[layers->size()-1][from_node];
    unsigned to_partition = (*layers)[layers->size()-1][to_node];
    for (unsigned i = layers->size()-2; i > 0; i--)
    { 
        if ((*layers)[i][from_partition] != (*layers)[i][to_partition])
        {
            //cout << "i: " << i << ", from: " << from_partition << ", to: " << to_partition << endl;
            //cout << "+ " << (*distance_table)[i][from_partition].outbound_distance << endl;
            //cout << "+ " << (*distance_table)[i][to_partition].inbound_distance << endl;
            // both nodes are in partitions in separate bound at level i
            future_weight += (*distance_table)[i][from_partition].outbound_distance;
            future_weight += (*distance_table)[i][to_partition].inbound_distance;
        }
        else
        {
            // both nodes are in partitions in the same bound at level i
            //cout << "+ " << (*distance_table)[i][from_partition].partition_distance[to_partition] << endl;
            future_weight += (*distance_table)[i][from_partition].partition_distance[to_partition];
            break;
        }
        // move up one level       
        from_partition = (*layers)[i][from_partition];
        to_partition = (*layers)[i][to_partition];
    }
    return future_weight;
}

/*
 * Weight retrieval function calculating realized weight created by travelling an edge starting at a time.
 * Parameters: const unsigned & edge
 *             const time_t & enter_time
 * Return: w'(e, t) -> expected time needed to finish travelling this edge
 */
double IMS::Router::retrieve_realized_weight(const unsigned &edge, const time_t &enter_time)
{
    // travel-time w.r.t. current traffic density ONLY
    double occupancy = map_graph->find_current_density(edge, enter_time) / map_graph->max_density;
    double basic_weight = map_graph->default_travel_time[edge] / (1 - occupancy);

    // a(e): incident detection
    double time_dependent_modifier = incident_manager->get_total_incident_impact(edge);

    return basic_weight + time_dependent_modifier;
}

IMS::Path* IMS::Router::route(const unsigned &origin, const unsigned &destination, const time_t &start_time)
{
    /*
    time_t start_time;
    time_t end_time;
    map<time_t, unsigned> enter_times; // <enter_time, edge ID>
    vector<double[2]> nodes; // [[long, lat]]
    */
    

    // A* search
    // prepare storage for single source graph search
    vector<unsigned> dist(map_graph->first_out.size(), INFINITY);
    vector<unsigned> prev(map_graph->first_out.size(), INFINITY); // infinity defined as nil here
    priority_queue<pair<unsigned, pair<unsigned, time_t>>, vector<pair<unsigned, pair<unsigned, time_t>>>, greater<pair<unsigned, pair<unsigned, time_t>>>> open;
    //set<unsigned> closed;

    open.push(make_pair(0, make_pair(origin, start_time)));
    dist[origin] = 0;

    // process the node u with minimum dist[u]
    while (!open.empty())
    {
        unsigned current_node = open.top().second.first;
        time_t current_node_time = open.top().second.second;
        open.pop();
        
        // premature end the graph search if target reached
        if (current_node == destination)
        {
            // reverse path
            stack<unsigned> node_stack;
            unsigned node = destination;
            while (node != origin)
            {
                node_stack.push(node);
                node = prev[node];
            }

            // retreve information
            IMS::Path* path = new IMS::Path();
            path->start_time = start_time;
            time_t time = start_time;

            while (node_stack.size() != 1)
            {
                unsigned this_node = node_stack.top();
                node_stack.pop();
                unsigned next_node = node_stack.top();
                unsigned edge = map_graph->find_edge(this_node, next_node);

                double node_coor[] = {(map_graph->longitude[this_node]), (map_graph->latitude[this_node])}; 
                //path->nodes.push_back(node_coor);
                path->enter_times[time] = edge;

                time = time + retrieve_realized_weight(edge, time);
            }
            path->end_time = time;

            return path;
        }

        // expand neighbours
        unsigned first_edge = map_graph->first_out[current_node];
        unsigned last_edge = (current_node == map_graph->first_out.size() -1) ? (map_graph->head.size()) : map_graph->first_out[current_node + 1];
        for (unsigned int current_edge = first_edge; current_edge < last_edge; current_edge ++)
        {
            unsigned next_node = map_graph->head[current_edge];
            unsigned g = dist[current_node];
            unsigned h = retrieve_future_weight(next_node, destination);
            unsigned w = retrieve_realized_weight(current_edge, current_node_time);

            unsigned f = g + h + w;
            if (dist[next_node] > f)
            {
                dist[next_node] = f;
                prev[next_node] = current_node;
                open.push(make_pair(dist[next_node], make_pair(next_node, current_node_time + w)));
            }
        }
    }


    IMS::Path* path = new IMS::Path();
    return path;
}