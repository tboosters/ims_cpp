//
// Created by terchow on 12/6/18.
//

#include <iostream>
#include <vector>
#include "../include/ims/map_graph.h"

using namespace std;
using namespace IMS;


const vector<float> &MapGraph::get_latitude() const
{
    return latitude;
}

const vector<float> &MapGraph::get_longitude() const
{
    return longitude;
}

const vector<unsigned int> &MapGraph::get_head() const
{
    return head;
}

const vector<unsigned int> &MapGraph::get_first_out() const
{
    return first_out;
}

const vector<unsigned int> &MapGraph::get_travel_time() const
{
    return travel_time;
}

void MapGraph::set_latitude(const vector<float> &latitude)
{
    MapGraph::latitude = latitude;
}

void MapGraph::set_longitude(const vector<float> &longitude)
{
    MapGraph::longitude = longitude;
}

void MapGraph::set_head(const vector<unsigned int> &head)
{
    MapGraph::head = head;
}

void MapGraph::set_first_out(const vector<unsigned int> &first_out)
{
    MapGraph::first_out = first_out;
}

void MapGraph::set_travel_time(const vector<unsigned int> &travel_time)
{
    MapGraph::travel_time = travel_time;
}
