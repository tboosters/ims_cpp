#include <iostream>
#include <assert.h>
#include <vector>

#include "../include/ims/incident_manager.h"

using namespace std;

int main()
{
    auto incidentManager = new IMS::IncidentManager();

    /* Test can increment num_of_incident and return incident ID */
    vector<unsigned> edge1{42};
    vector<unsigned> edge2{0, 1};
    assert(incidentManager->add_incident(edge1, 2) == 0);
    assert(incidentManager->add_incident(edge1, 4) == 1);

    /* Test can insert to more than 1 edge */
    incidentManager->add_incident(edge2, 4);
    assert(incidentManager->get_total_incident_impact(0) == 4);
    assert(incidentManager->get_total_incident_impact(1) == 4);

    /* Test can add to affected_roads */
    assert(incidentManager->get_total_incident_impact(42) == 6);

    /* Test can remove incident */
    incidentManager->remove_incident(1);
    assert(incidentManager->get_total_incident_impact(42) == 2);
}

