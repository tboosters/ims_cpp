/*
 * Manages incident information including IDs, impact and affected roads.
 * Provides adding, removing and retrieval operations.
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow
 */

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../include/ims/incident_manager.h"

using namespace std;

/* Stores incident with impact. Assign it with an ID and stores ID associated with edges.
 * Number of incident is incremented.
 *
 * Parameter(s): vector<unsigned> edge_id
 *               unsigned impact
 * Returns: unsigned: incident ID
 */
unsigned IMS::IncidentManager::add_incident(vector<unsigned> affected_edges, unsigned impact)
{
    // Lock exclusive writer access
    boost::unique_lock<boost::shared_mutex> writer_lock(access);

    unsigned incident_id = num_of_incident;
    incidents[incident_id] = impact;
    for(auto & edge : affected_edges)
    {
        affected_roads[edge].insert(incident_id);
    }
    num_of_incident++;
    return incident_id;
}

/* Removes incident. Finds all roads affected by this incident and remove entries.
 * Removes also the affected road entry if the entry is empty.
 *
 * Parameter(s): unsigned incident_id
 * Returns: unsigned: number of incident removed -> 0 indicates incident ID not found
 */
unsigned IMS::IncidentManager::remove_incident(unsigned incident_id)
{
    // Lock exclusive writer access
    boost::unique_lock<boost::shared_mutex> writer_lock(access);

    unsigned num_of_incidents_removed = incidents.erase(incident_id);

    if(num_of_incidents_removed > 0)
    {
        vector<unordered_map<unsigned, unordered_set<unsigned> >::iterator>
                affected_road_entries_to_be_removed;
        for(auto itr = affected_roads.begin(); itr != affected_roads.end(); itr++)
        {
            if(itr->second.erase(incident_id) > 0 && itr->second.empty())
            {
                // something is erased and set is empty after that
                affected_road_entries_to_be_removed.push_back(itr);
            }
        }
        for(auto entry : affected_road_entries_to_be_removed)
        {
            affected_roads.erase(entry);
        }
    }

    return num_of_incidents_removed;
}

/* Calculate total impact brought by incidents on the edge specified.
 * Parameter(s): unsigned edge_id
 * Returns: double: total incident impact
 */
double IMS::IncidentManager::get_total_incident_impact(unsigned edge_id)
{
    // Lock reader access
    boost::shared_lock<boost::shared_mutex> reader_lock(access);

    if(affected_roads.count(edge_id) == 0)
    {
        return 0;
    }

    double total_incident_impact = 0;
    for(auto & incident : affected_roads[edge_id])
    {
        total_incident_impact += incidents[incident];
    }
    return total_incident_impact;
}