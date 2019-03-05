/*
 * Routing functions. All functions are free functions in IMS::Routing namespace.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow
 */

#include "../include/ims/router.h"

/* Future weight retrieval function, calculate the estimated future weight between two nodes.
 * Parameters: const unsigned from_node
 *             const unsigned to_node
 * Return: h(u, t) -> estimated time needed to travel from u to t
 */
unsigned IMS::Router::retrieve_future_weight(const unsigned from_node, const unsigned to_node)
{
    IMS::Preprocess::distance_table_t* distance_table = map_graph->distance_tables;
    IMS::Partition::layer_t* layers = map_graph->layers;
    unsigned from_partition = from_node;
    unsigned to_partition = to_node;
    unsigned future_weight = 0;
    for (unsigned i = layers->size()-1; i > 0; i--)
    {
        // move up one level
        from_partition = (*layers)[i][from_partition];
        to_partition = (*layers)[i][to_partition];
        if (from_partition != to_partition)
        {
            // both nodes are in partitions in seperate bound at level i
            future_weight += (*distance_table)[i][from_partition].outbound_distance;
            future_weight += (*distance_table)[i][to_partition].inbound_distance;
        }
        else
        {
            // both nodes are in partitions in the same bound at level i
            future_weight += (*distance_table)[i][from_partition].partition_distance[to_partition];
            break;
        }
    }

    return future_weight;
}

/*
 * Weight retrieval function calculating new weight created by travelling an edge starting at a time.
 * Parameters: IMS::MapGraph * map_graph: graph data
 *             const unsigned & edge
 *             const time_t & enter_time
 * Return: w'(e, t) -> expected time needed to finish travelling this edge
 */
double IMS::Router::retrieve_weight(const unsigned & edge, const time_t & enter_time)
{
    // travel-time w.r.t. current traffic density ONLY
    double occupancy = map_graph->find_current_density(edge, enter_time) / map_graph->max_density;
    double basic_weight = map_graph->default_travel_time[edge] / (1 - occupancy);

    // a(e): incident detection
    double time_dependent_modifier = incident_manager->get_total_incident_impact(edge);

    return basic_weight + time_dependent_modifier;
}