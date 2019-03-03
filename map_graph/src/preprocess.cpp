/*
 * Preprocess functions. All functions are free functions in IMS::Routing namespace.
 * Libraries:
 * Version: 1.0
 * Author: Yuen Hoi Man
 */

#include <iostream>
#include <cmath>
#include <queue>
#include <set>

#include "preprocess.h"

using namespace std;
using namespace IMS::Preprocess;

// helper function
/* Perform single source shortest distance search from an extra node to and extra node
 * Parameters: const IMS:MapGraph* graph
 *             vector<unsigned>* extra_outward_edges : temporary edges from the origin node
 *             unordered_map<unsigned, unsigned>* extra_inward_edges : temporary edges to the target node
 */
unsigned modified_shortest_destance(const IMS::MapGraph* graph, vector<unsigned>* extra_outward_edges, 
                                    unordered_map<unsigned, unsigned>* extra_inward_edges)
{
    // prepare storage for single source graph search
    vector<unsigned> dist(graph->first_out.size() + 2, INFINITY);
    vector<unsigned> prev(graph->first_out.size() + 2, INFINITY); // infinity defined as nil here
    set<unsigned> s;
    priority_queue<pair<unsigned, unsigned>, vector<pair<unsigned, unsigned>>, greater<pair<unsigned, unsigned>>> q;

    // insert new temp node into q
    unsigned origin = graph->first_out.size();
    unsigned target = graph->first_out.size() + 1;
    q.push(make_pair(0, origin));
    dist[origin] = 0;

    // process the node u with minimum dist[u]
    while (!q.empty())
    {
        unsigned u = q.top().second;
        q.pop();
        
        // premature end the graph search if target reached
        if (u = target)
        {
            return dist[u];
        }

        // expand neighbours
        if (u != origin)
        {
            unsigned first_edge = graph->first_out[u];
            unsigned last_edge = (u == graph->first_out.size() -1) ? (graph->head.size()) : graph->first_out[u + 1];
            for (unsigned int current_edge = first_edge; current_edge < last_edge; current_edge ++)
            {
                unsigned v = graph->head[current_edge];
                if (dist[v] > dist[u] + graph->default_travel_time[current_edge])
                {
                    dist[v] = dist[u] + graph->default_travel_time[current_edge];
                    prev[v] = u;
                }
            }
            if ((*extra_inward_edges).count(u) > 0)
            {
                unsigned v = target;
                if (dist[v] > dist[u] + 0)
                {
                    dist[v] = dist[u] + 0;
                    prev[v] = u;
                }
            }
        }
        else
        {
            for (unsigned int current_edge = 0; current_edge < (*extra_outward_edges).size(); current_edge ++)
            {
                unsigned v = (*extra_outward_edges)[current_edge];
                if (dist[v] > dist[u] + 0)
                {
                    dist[v] = dist[u] + 0;
                    prev[v] = u;
                }
            }
        }
    }
    return 0;
}



/* Precompute all distance information of partitions
 * Parameters: const vector<unsigned> & nodes
 *             const IMS::MapGraph * graph
 *             const IMS::Partition::partition_t* partitions
 *             const IMS::Pratition::layer_t* layers
 * Return: distance_table_t: distance table fully filled with distance information
 */
distance_table_t* IMS::Preprocess::do_preprocess (const vector<unsigned> &nodes, 
        const IMS::MapGraph* graph, IMS::Partition::partition_t* partitions, IMS::Partition::layer_t* layers)
{    
    // create working copy of the graph information
    /*
    vector<unsigned> temp_head(graph->head);
    vector<unsigned> temp_first_out(graph->first_out);
    vector<unsigned> temp_default_travel_time(graph->default_travel_time);
    */

    // initialize distance table
    distance_table_t* distance_table = new distance_table_t;
    for (unsigned int i = 1; i < layers->size(); i++)
    {
        // initialize distance table for layer i
        vector<entry_t> dti;
        distance_table->push_back(dti);
    }

    // extra information for partition traversal
    int level_node_count = 1;
    int new_level_node_count = 0;
    queue<IMS::Partition::partition_t*> frontier;
    IMS::Partition::partition_t * curr;
    frontier.push(partitions);

    // traverse the partitions
    while(!frontier.empty())
    {
        for(int i = 0; i < level_node_count; i++)
        {
            // Get current node
            curr = frontier.front(); // m
            frontier.pop();

            // inject new temporary vm nodes and edges
            unsigned vm = graph->first_out.size() + 1;
            vector<unsigned> vm_head;
            for (unsigned bound_node : curr->boundary_outwards)
            {
                vm_head.push_back(bound_node);
            }
            unordered_map<unsigned, unsigned> vm_head_inversed;
            for (unsigned bound_node : curr->boundary_inwards)
            {
                vm_head_inversed[bound_node] = 0;
            }

            // find distance between partition within same bound
            // prepare storage for single source graph search
            vector<unsigned> dist(graph->first_out.size() + 1, INFINITY);
            vector<unsigned> prev(graph->first_out.size() + 1, INFINITY); // infinity defined as nil here
            map<unsigned, unsigned> rep;
            for (auto partition_same_bound : curr->parent_partition->sub_partition)
            {
                rep[partition_same_bound->id] = INFINITY;
            }
            set<unsigned> s;
            priority_queue<pair<unsigned, unsigned>, vector<pair<unsigned, unsigned>>, greater<pair<unsigned, unsigned>>> q;

            // insert new temp node into q
            unsigned origin = vm;
            q.push(make_pair(0, origin));
            dist[origin] = 0;

            // process the node u with minimum dist[u]
            while (!q.empty())
            {
                unsigned u = q.top().second;
                q.pop();

                // regiester partition distances
                if (u != vm)
                {
                    unsigned c = u;
                    for (int layer = layers->size() - 1; layer > i; layer --)
                    {
                        c = (*layers)[layer][c];
                    }
                    if (rep[c] == INFINITY) 
                    {
                        rep[c] = u;
                    }
                }
                
                // premature end the graph search when all partition within the same bound searched
                bool all_filled = true;
                for (map<unsigned, unsigned>::iterator entry = rep.begin(); entry != rep.end(); entry++)
                {
                    if (entry->second != INFINITY)
                    {
                        all_filled = false;
                        break;
                    }
                }
                if (all_filled)
                {
                    break;
                }

                // expand neighbours
                if (u != vm)
                {
                    unsigned first_edge = graph->first_out[u];
                    unsigned last_edge = (u == graph->first_out.size() -1) ? (graph->head.size()) : graph->first_out[u + 1];
                    for (unsigned int current_edge = first_edge; current_edge < last_edge; current_edge ++)
                    {
                        unsigned v = graph->head[current_edge];
                        if (dist[v] > dist[u] + graph->default_travel_time[current_edge])
                        {
                            dist[v] = dist[u] + graph->default_travel_time[current_edge];
                            prev[v] = u;
                        }
                    }
                }
                else
                {
                    for (unsigned int current_edge = 0; current_edge < vm_head.size(); current_edge ++)
                    {
                        unsigned v = vm_head[current_edge];
                        if (dist[v] > dist[u] + 0)
                        {
                            dist[v] = dist[u] + 0;
                            prev[v] = u;
                        }
                    }
                }
            }

            // all distance information towards partitions within the same bound into distance table
            for (auto partition_same_bound : curr->parent_partition->sub_partition)
            {
                (*distance_table)[i][curr->id].partition_distance[partition_same_bound->id] = rep[partition_same_bound->id];
            }

            // find distance to and from the bound borders
            // inject new temporary vM nodes and edges
            unsigned vM = vm + 1;
            vector<unsigned> vM_head;
            for (unsigned bound_node : curr->parent_partition->boundary_outwards)
            {
                vM_head.push_back(bound_node);
            }
            unordered_map<unsigned, unsigned> vM_head_inversed;
            for (unsigned bound_node : curr->parent_partition->boundary_inwards)
            {
                vM_head_inversed[bound_node] = 0;
            }

            // calculate distances
            (*distance_table)[i][curr->id].outbound_distance = modified_shortest_destance(graph, &vm_head, &vM_head_inversed);
            (*distance_table)[i][curr->id].outbound_distance = modified_shortest_destance(graph, &vM_head, &vm_head_inversed);

            // remove temp node and vertex
            // -- due to implementation, nothing need to be done

            // Expand neighbour
            for(auto sp : curr->sub_partition)
            {
                frontier.push(sp);
            }
            new_level_node_count += curr->sub_partition.size();
        }
        level_node_count = new_level_node_count;
        new_level_node_count = 0;
    }
    return distance_table;
}
