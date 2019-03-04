/*
 * Routing functions. All functions are free functions in IMS::Routing namespace.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow
 */

#include "../include/ims/router.h"

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