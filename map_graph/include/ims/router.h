/*
 * Header file for routing module.
 * Version: 1.0
 * Author: Terence Chow & Yuen Hoi Man
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
    const unsigned JAMMED_WEIGHT = 172800000; // 48 hours in milliseconds
    IMS::MapGraph * map_graph;
    IMS::IncidentManager * incident_manager;

public:
    Router(IMS::MapGraph * mg, IMS::IncidentManager * im) : map_graph(mg), incident_manager(im) {};

    unsigned retrieve_future_weight(const unsigned &from_node, const unsigned &to_node);
    unsigned int retrieve_realized_weight(const unsigned &edge, const time_t &enter_time);

    IMS::Path* route(const unsigned &origin, const unsigned &destination, const time_t &start_time);


};
}


#endif //IMS_CPP_ROUTING_H
