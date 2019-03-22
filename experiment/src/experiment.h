#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <string>
#include "../include/exp_log.h"

void experiment_route(string filename, string graph, float origin_long, float origin_lat, float destination_long, float destination_lat, float radius);
void create_xml(string filename, ExpandedLog* log);


#endif