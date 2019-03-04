/*
 * Header file for routing module.
 * Version: 1.0
 * Author: Terence Chow
 */

#ifndef IMS_CPP_ROUTING_H
#define IMS_CPP_ROUTING_H


#include <ctime>
#include "map_graph.h"
#include "incident_manager.h"

namespace IMS
{
class Router
{
private:
    IMS::MapGraph * map_graph;
    IMS::IncidentManager * incident_manager;

public:
    Router(IMS::MapGraph * mg, IMS::IncidentManager * im) : map_graph(mg), incident_manager(im) {};

    double retrieve_weight(const unsigned & edge, const time_t & enter_time);

};
}


#endif //IMS_CPP_ROUTING_H
