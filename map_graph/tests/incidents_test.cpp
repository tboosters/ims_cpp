#include <iostream>
#include <assert.h>

#include "../src/incident_manager.h"

using namespace std;

int main()
{
    auto incidentManager = new IMS::IncidentManager();

    /* Test can increment num_of_incident and return incident ID */
    assert(incidentManager->add_incident(42, 2.1) == 0);
    assert(incidentManager->add_incident(42, 4.2) == 1);

    /* Test can add to affected_roads */
    double expected = 2.1 + 4.2;  // Calculate expected value here to avoid float point problem (6.3 != 6.3).
    assert(incidentManager->get_total_incident_effect(42) == expected);

    /* Test can remove incident */
    incidentManager->remove_incident(1);
    assert(incidentManager->get_total_incident_effect(42) == 2.1);
}

