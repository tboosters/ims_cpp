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
/* Perform single source shortest distance search from a set of nodes to a set nof nodes
 * Parameters: const IMS:MapGraph* graph
 *             vector<unsigned>* from_nodes : origin node set
 *             vector<unsigned>* to_nodes : target node set
 */
unsigned nodeset_shortest_destance(const IMS::MapGraph* graph, vector<unsigned>* from_nodes, 
                                    vector<unsigned>* to_nodes)
{    
    // create node set for to nodes
    set<unsigned> to_set;
    for(auto n : (*to_nodes))
    {
        to_set.insert(n);
    }

    // prepare storage for single source graph search
    vector<unsigned> dist(graph->first_out.size() + 2, INFINITY);
    vector<unsigned> prev(graph->first_out.size() + 2, INFINITY); // infinity defined as nil here
    priority_queue<pair<unsigned, unsigned>, vector<pair<unsigned, unsigned>>, greater<pair<unsigned, unsigned>>> q;

    for (auto n : (*from_nodes))
    {
        q.push(make_pair(0, n));
        dist[n] = 0;
    }

    // process the node u with minimum dist[u]
    while (!q.empty())
    {
        unsigned u = q.top().second;
        q.pop();
        
        // premature end the graph search if target reached
        if (to_set.count(u) > 0)
        {
            return dist[u];
        }

        // expand neighbours
        unsigned first_edge = graph->first_out[u];
        unsigned last_edge = (u == graph->first_out.size() -1) ? (graph->head.size()) : graph->first_out[u + 1];
        for (unsigned int current_edge = first_edge; current_edge < last_edge; current_edge ++)
        {
            unsigned v = graph->head[current_edge];
            if (dist[v] > dist[u] + graph->default_travel_time[current_edge])
            {
                dist[v] = dist[u] + graph->default_travel_time[current_edge];
                prev[v] = u;
                q.push(make_pair(dist[v], v));
            }
        }
    }
    return INFINITY;
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
    // initialize distance table
    distance_table_t* distance_table = new distance_table_t;

    for (unsigned int i = 0; i < layers->size() - 1; i++)
    {
        // initialize distance table for layer i
        vector<entry_t> dti;
        for (unsigned j = 0; j < (*layers)[i].size(); j++)
        {
            entry_t entry;
            entry.inbound_distance = INFINITY;
            entry.outbound_distance = INFINITY;
            dti.push_back(entry);
        }
        distance_table->push_back(dti);
    }

    // extra information for partition traversal
    int level_node_count = partitions->sub_partition.size();
    int new_level_node_count = 0;
    queue<IMS::Partition::partition_t*> frontier;
    IMS::Partition::partition_t * curr;
    for (auto p : partitions->sub_partition)
    {
        frontier.push(p);
    }

    // traverse the partitions
    while(!frontier.empty())
    {
        for(int i = 0; i < level_node_count; i++)
        {
            // Get current node
            curr = frontier.front(); // m
            frontier.pop();

            // inject new temporary vm nodes and edges
            unsigned vm = graph->first_out.size();
            vector<unsigned> vm_head;
            for (unsigned bound_node : curr->boundary_outwards)
            {
                vm_head.push_back(bound_node);
            }

            // find distance between partition within same bound
            // prepare storage for single source graph search
            vector<unsigned> dist(graph->first_out.size() + 1, INFINITY);
            vector<unsigned> prev(graph->first_out.size() + 1, INFINITY); // infinity defined as nil here
            map<unsigned, unsigned> rep;
            for (auto partition_same_bound : curr->parent_partition->sub_partition)
            {
                rep[partition_same_bound->id] = INFINITY;
                //cout << "initial reg p: " << partition_same_bound->id << " = " << rep[partition_same_bound->id] << endl;
            }
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
                    for (int layer = layers->size() - 1; layer > curr->layer; layer --)
                    {
                        c = (*layers)[layer][c];
                    }
                    if (rep[c] == (unsigned)INFINITY) 
                    {
                        //cout << "regestered p: " << c << endl;
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
                            q.push(make_pair(dist[v], v));
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
                            q.push(make_pair(dist[v], v));
                        }
                    }
                }
            }

            // all distance information towards partitions within the same bound into distance table
            for (auto partition_same_bound : curr->parent_partition->sub_partition)
            {
                //cout << "saving at level " << curr->layer << ", from p: " << curr->id << ", to p: " << partition_same_bound->id << ", dist: " << rep[partition_same_bound->id] << endl;
                if (rep[partition_same_bound->id] != (unsigned)INFINITY)
                {   
                    (*distance_table)[curr->layer][curr->id].partition_distance[partition_same_bound->id] = dist[rep[partition_same_bound->id]];
                }
            }

            // find distance to and from the bound borde
            (*distance_table)[curr->layer][curr->id].outbound_distance = nodeset_shortest_destance(graph, &(curr->boundary_outwards),&(curr->parent_partition->boundary_outwards));
            (*distance_table)[curr->layer][curr->id].inbound_distance = nodeset_shortest_destance(graph, &(curr->parent_partition->boundary_inwards), &(curr->boundary_inwards));

            // remove temp node and vertex
            // -- due to implementation, nothing need to be done

            // Expand neighbour
            for(auto sp : curr->sub_partition)
            {
                if (sp->is_node == false)
                {
                    frontier.push(sp);
                    new_level_node_count += 1;
                }
            }
        }

        level_node_count = new_level_node_count;
        new_level_node_count = 0;
    }
    
    return distance_table;
}

/* Print the distance table structure.
 * Parameter: distance_table_t * distance_table
 * Return: when distance tables is printed
 */
void IMS::Preprocess::print_distance_table (IMS::Preprocess::distance_table_t * distance_table)
{
    for (int i = 1; i < (*distance_table).size(); i++)
    {
        cout << "Level " << i << ":" << endl;
        for (int curr = 0; curr < (*distance_table)[i].size(); curr ++)
        {
            cout << curr << " ";
            for (map<unsigned, unsigned>::iterator target = (*distance_table)[i][curr].partition_distance.begin(); 
                        target != (*distance_table)[i][curr].partition_distance.end(); target++)
            {
                cout << target->first << "(" << target->second << ") ";
            }
            cout << "out(" << (*distance_table)[i][curr].outbound_distance << ") ";
            cout << "in(" << (*distance_table)[i][curr].inbound_distance << ") | ";
        }
        cout << endl;
    }
}

