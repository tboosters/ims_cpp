#include <random>
#include <fstream>
#include <vector>

#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/http_response.h>

#include "utils.h"
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
    /* Load sample_route.json */
    ifstream sample_route_data(get_exec_dir() + "/sample_route.json");
    cppcms::json::value all_routes;
    all_routes.load(sample_route_data, true);
    sample_route_data.close();

    /* Randomly pick a route */
    random_device rd;
    default_random_engine engine(rd());
    uniform_int_distribution<int> uniform_dist(0, 100);
    cppcms::json::value route = all_routes[uniform_dist(engine)];

    /* Write route to response */
    cppcms::json::value response_body;
    response_body["data"] = route;
    response().out() << response_body;
}

void IMSApp::inject_incident()
{
    response().out() << "inject_incident";
};


void IMSApp::handle_graph_update()
{
    response().out() << "handle_graph_update";
};
