/*
 * Header file for routing module.
 * Version: 1.0
 * Author: Terence Chow
 */

#ifndef IMS_CPP_ROUTING_H
#define IMS_CPP_ROUTING_H


#include <ctime>
#include <ims/map_graph.h>

namespace IMS
{
namespace Routing
{

double retrieve_weight(IMS::MapGraph * map_graph, const unsigned & edge, const time_t & enter_time);

}
}


#endif //IMS_CPP_ROUTING_H
