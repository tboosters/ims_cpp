/*
 * Header file for IncidentManager
 * Libraries:
 * Version: 1.0
 * Author: Terence Chow
 */

#ifndef IMS_CPP_INCIDENTS_H
#define IMS_CPP_INCIDENTS_H

#include <unordered_map>
#include <unordered_set>

#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>

using namespace std;

namespace IMS
{

class IncidentManager
{
private:
    boost::shared_mutex access;
    unsigned num_of_incident = 0;
    unordered_map<unsigned, unsigned> incidents;
    unordered_map<unsigned, unordered_set<unsigned> > affected_roads;

public:
    unsigned add_incident(vector<unsigned> affected_edges, unsigned impact);
    unsigned remove_incident(unsigned incident_id);
    double get_total_incident_impact(unsigned edge_id);
};

}


#endif //IMS_CPP_INCIDENTS_H
