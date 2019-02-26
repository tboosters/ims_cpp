#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "incident_manager.h"

using namespace std;

unsigned IMS::IncidentManager::add_incident(unsigned edge_id, double impact)
{
    unsigned incident_id = num_of_incident;
    incidents[incident_id] = impact;
    affected_roads[edge_id].insert(incident_id);
    num_of_incident++;
    return incident_id;
}

void IMS::IncidentManager::remove_incident(unsigned incident_id)
{
    incidents.erase(incident_id);

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

double IMS::IncidentManager::get_total_incident_effect(unsigned edge_id)
{
    double total_incident_effect = 0;
    for(auto & incident : affected_roads[edge_id])
    {
        total_incident_effect += incidents[incident];
    }
    return total_incident_effect;
}