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
        //cout << "checking i: " << i << ", " << (*layers)[i][from_partition] << " == " << (*layers)[i][to_partition] << endl;
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
unsigned int IMS::Router::retrieve_realized_weight(const unsigned &edge, const time_t &enter_time)
{
    // travel-time w.r.t. current traffic density ONLY
    double occupancy = map_graph->find_current_density(edge, enter_time) / map_graph->max_density;
    if(occupancy >= 0.8)
    {
        return 5 * map_graph->default_travel_time[edge];
    }

    double basic_weight = map_graph->default_travel_time[edge] / (1 - occupancy);

    // a(e): incident detection
    double time_dependent_modifier = incident_manager->get_total_incident_impact(edge);

    return round(basic_weight + time_dependent_modifier);
}

IMS::Path* IMS::Router::route(const unsigned &origin, const unsigned &destination, const time_t &start_time,  ExpandedLog* log)
{
    // A* search
    // prepare storage for single source graph search
    vector<unsigned> dist(map_graph->first_out.size(), INFINITY);
    vector<unsigned> prev(map_graph->first_out.size(), INFINITY); // infinity defined as nil here
    priority_queue<
            pair<unsigned, pair<unsigned, time_t>>,
            vector<pair<unsigned, pair<unsigned, time_t>>>,
            greater<pair<unsigned, pair<unsigned, time_t>>>
            > open;
    open.push(make_pair(0, make_pair(origin, start_time * 1000))); // Covert start_time to millisecond
    dist[origin] = 0;

    // data structure only for logging
    priority_queue<
            pair<unsigned, tuple<unsigned, unsigned, unsigned, unsigned ,unsigned>>,
            vector<pair<unsigned, tuple<unsigned, unsigned, unsigned, unsigned ,unsigned>>>,
            greater<pair<unsigned, tuple<unsigned, unsigned, unsigned, unsigned ,unsigned>>>
            > open_log;
    open_log.push(make_pair(0, make_tuple(origin, origin, 0, 0, 0)));


    // process the node u with minimum dist[u]
    while (!open.empty())
    {
        unsigned current_node = open.top().second.first;
        time_t current_node_time = open.top().second.second;
        open.pop();

        // log the search space if appicable
        if (log != NULL)
        {   
            unsigned current_node = get<0>(open_log.top().second);
            unsigned pass_node = get<1>(open_log.top().second);
            unsigned g = get<2>(open_log.top().second);
            unsigned h = get<3>(open_log.top().second);
            unsigned w = get<4>(open_log.top().second);
            unsigned f = g + h + w;

            unsigned layer_info[5];
            for (int i = 0; i < 5; i++)
            {
                layer_info[i] = -1;
            }
            unsigned l = current_node;
            //unsigned l = (*(map_graph->layers))[layers->size()-1][current_node];
            for (unsigned i = map_graph->layers->size()-1; i > 0; i--)
            { 
                //cout << i << ", " << l << endl;
                layer_info[i] = l;
                //layer_info.emplace_back(i, (*(map_graph->layers))[i][l]);
                // move up one level       
                l = (*(map_graph->layers))[i][l];
            }        

            log->expanded_nodes[pass_node] = make_pair(map_graph->latitude[pass_node], map_graph->longitude[pass_node]);
            log->expanded_nodes[current_node] = make_pair(map_graph->latitude[current_node], map_graph->longitude[current_node]);
            log->expanded_edges.emplace_back(pass_node, current_node, g, h, w, f, layer_info[0], layer_info[1], layer_info[2], layer_info[3], layer_info[4]);
            open_log.pop();
        }

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
            node_stack.push(origin);

            // retreve information
            IMS::Path* path = new IMS::Path();
            path->start_time = start_time * 1000;
            time_t time = start_time * 1000;

            while (node_stack.size() != 1)
            {
                unsigned this_node = node_stack.top();
                node_stack.pop();
                //cout << "[" << this_node << "] -> ";
                unsigned next_node = node_stack.top();
                unsigned edge = map_graph->find_edge(this_node, next_node);

                path->nodes.emplace_back(map_graph->longitude[this_node], map_graph->latitude[this_node]);
                path->enter_times[time] = edge;

                time = time + retrieve_realized_weight(edge, time);

                // log final path if appicable
                if (log != NULL)
                {
                    log->path_edges.emplace_back(this_node, next_node);
                }
            }
            path->end_time = time;
            path->nodes.emplace_back(map_graph->longitude[destination], map_graph->latitude[destination]);
            //cout << node_stack.top() << "|" << endl;

            return path;
        }
  

        // expand neighbours
        unsigned first_edge = map_graph->first_out[current_node];
        unsigned last_edge = (current_node == map_graph->first_out.size() -1) ? (map_graph->head.size()) : map_graph->first_out[current_node + 1];;
        for (unsigned int current_edge = first_edge; current_edge < last_edge; current_edge ++)
        {
            unsigned next_node = map_graph->head[current_edge];
            unsigned g = dist[current_node];
            unsigned h = retrieve_future_weight(next_node, destination);
            unsigned w = retrieve_realized_weight(current_edge, current_node_time);
//            unsigned w = map_graph->default_travel_time[current_edge];

            unsigned f = g + h + w;
            if (dist[next_node] > g + w)
            {
                dist[next_node] = g + w;
                prev[next_node] = current_node;
                //open.push(make_pair(dist[next_node], make_pair(next_node, current_node_time + w)));
                open.push(make_pair(f, make_pair(next_node, current_node_time + w)));
                
                // log the search space if applicable
                if (log != NULL)
                {
                    open_log.push(make_pair(f, make_tuple(next_node, current_node, g, h, w)));
                }
            }
        }
    }

    return NULL;
}