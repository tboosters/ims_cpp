#ifndef IMS_CPP_INCIDENTS_H
#define IMS_CPP_INCIDENTS_H

#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace IMS
{

class IncidentManager
{
private:
    unsigned num_of_incident = 0;
    unordered_map<unsigned, unsigned> incidents;
    unordered_map<unsigned, unordered_set<unsigned> > affected_roads;

public:
    unsigned add_incident(unsigned edge_id, unsigned impact);
    unsigned remove_incident(unsigned incident_id);
    double get_total_incident_effect(unsigned edge_id);
};

}


#endif //IMS_CPP_INCIDENTS_H
