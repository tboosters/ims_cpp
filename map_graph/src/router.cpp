/*
 * Routing functions. All functions are free functions in IMS::Routing namespace.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow
 */

#include "../include/ims/router.h"

/* Future weight retrieval function, calculate the estimated future weight (heuristics) between two nodes.
 * Parameters: const unsigned from_node
 *             const unsigned to_node
 * Return: h(u, t) -> estimated time needed to travel from u to t
 */
unsigned IMS::Router::retrieve_future_weight(const unsigned from_node, const unsigned to_node)
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