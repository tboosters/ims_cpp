//
// Created by terchow on 12/5/18.
//

#include <vector>

#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/http_response.h>

#include "IMSApp.h"
#include "ims/map_graph.h"

using namespace std;
using namespace IMS;

IMSApp::IMSApp(cppcms::service &srv, IMS::MapGraph *map_graph) : cppcms::application(srv)
{
    this->map_graph = map_graph;

    // Dev url for checking graph
    dispatcher().map("GET", "/graph", &IMSApp::check_graph, this);

    // Add url dispatchers
    dispatcher().map("POST", "/route", &IMSApp::route, this);
    dispatcher().map("POST", "/incident", &IMSApp::inject_incident, this);
    dispatcher().map("POST", "/update_graph", &IMSApp::handle_graph_update, this);
}

void IMSApp::check_graph()
{
    response().out() << "Node Count: " << map_graph->get_latitude().size();
    response().out() << "<br>";
    response().out() << "Edge Count: " << map_graph->get_head().size();
}

void IMSApp::route()
{
    const int N = 20;
    double data[N][2] =
    {
        {-122.400129, 37.788975},
        {-122.399362, 37.788361},
        {-122.418702, 37.773081},
        {-122.41993, 37.770194},
        {-122.426132, 37.76965},
        {-122.434868, 37.762785},
        {-122.439417, 37.76139},
        {-122.438936, 37.756525},
        {-122.441021, 37.756218},
        {-122.444549, 37.747129},
        {-122.451624, 37.745658},
        {-122.454437, 37.743925},
        {-122.456246, 37.74176},
        {-122.458209, 37.740771},
        {-122.464528, 37.739935},
        {-122.466244, 37.739249},
        {-122.468337, 37.740036},
        {-122.469999, 37.739565},
        {-122.505448, 37.738003},
        {-122.50541, 37.73746}
    };

    cppcms::json::array route;
    for(auto & node : data)
    {
        cppcms::json::array elem;
        elem.push_back(node[0]);
        elem.push_back(node[1]);
        route.push_back(elem);
    }

    response().out() << route;
}

void IMSApp::inject_incident()
{
    response().out() << "inject_incident";
};


void IMSApp::handle_graph_update()
{
    response().out() << "handle_graph_update";
};
