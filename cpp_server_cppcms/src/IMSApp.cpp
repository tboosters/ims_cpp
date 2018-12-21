//
// Created by terchow on 12/5/18.
//

#include <stack>

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
    response().out() << "route";
}

void IMSApp::inject_incident()
{
    response().out() << "inject_incident";
};


void IMSApp::handle_graph_update()
{
    response().out() << "handle_graph_update";
};
