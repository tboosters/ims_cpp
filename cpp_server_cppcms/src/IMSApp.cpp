#include <random>
#include <fstream>
#include <vector>

#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <sstream>
#include <cstdlib>

#include "utils.h"
#include "IMSApp.h"
#include "ims/map_graph.h"
#include "ims/incident_manager.h"


using namespace std;
using namespace IMS;

// Util function for handling JSON
cppcms::json::value extract_json_data(pair<void *,size_t> raw_post_data) throw (booster::invalid_argument)
{
    cppcms::json::value json_data;

    if(raw_post_data.second <= 0)
    {
        throw booster::invalid_argument("Empty request body");
    }

    istringstream isstream(string(
            reinterpret_cast<char const*>(raw_post_data.first),
            raw_post_data.second));

    if(!json_data.load(isstream, true))
    {
        throw booster::invalid_argument("Invalid JSON");
    }

    return json_data;
}

IMSApp::IMSApp(cppcms::service &srv, IMS::MapGraph *map_graph, IMS::IncidentManager *incident_manager) : cppcms::application(srv)
{
    this->map_graph = map_graph;
    this->incident_manager = incident_manager;

    // Dev url for checking graph
    dispatcher().map("GET", "/graph", &IMSApp::check_graph, this);

    // Add url dispatchers
    dispatcher().map("POST", "/route", &IMSApp::route, this);
    dispatcher().map("POST", "/incident", &IMSApp::inject_incident, this);
    dispatcher().map("DELETE", "/incident", &IMSApp::remove_incident, this);
    dispatcher().map("POST", "/update_graph", &IMSApp::handle_graph_update, this);
}

void IMSApp::check_graph()
{
    response().out() << "Node Count: " << map_graph->latitude.size();
    response().out() << "<br>";
    response().out() << "Edge Count: " << map_graph->head.size();
}

void IMSApp::route()
{
    /* Take POST JSON body */
    cppcms::json::value json_data;
    try
    {
         json_data = extract_json_data(request().raw_post_data());
    }
    catch (booster::invalid_argument & e)
    {
        response().make_error_response(400, e.what());
        return;
    }

    double origin_long = json_data["coordinates"][0][0].number();
    double origin_lat = json_data["coordinates"][0][1].number();
    double destination_long = json_data["coordinates"][1][0].number();
    double destination_lat = json_data["coordinates"][1][1].number();

    /* Reverse Geocoding for origin and destination */
    unsigned origin =
            map_graph->map_geo_position.find_nearest_neighbor_within_radius(origin_lat, origin_long, 500).id;
    unsigned destination =
            map_graph->map_geo_position.find_nearest_neighbor_within_radius(destination_lat, destination_long, 500).id;
    if(origin == RoutingKit::invalid_id)
    {
        response().make_error_response(404, "No node within 500m from origin position.");
        return;
    }
    if(destination == RoutingKit::invalid_id)
    {
        response().make_error_response(404, "No node within 500m from destination position.");
        return;
    }

    printf("%f, %f\n%f, %f\n%d, %d", origin_long, origin_lat, destination_long, destination_lat, origin, destination);

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
    /* Take POST JSON body */
    cppcms::json::value json_data;
    try
    {
        json_data = extract_json_data(request().raw_post_data());
    }
    catch (booster::invalid_argument & e)
    {
        response().make_error_response(400, e.what());
        return;
    }

    double incident_long = json_data["location"][0].number();
    double incident_lat = json_data["location"][1].number();
    unsigned impact = (unsigned) json_data["impact"].number();

    /* Reverse Geocoding for affected edge */
    unsigned affected_edge = map_graph->find_nearest_edge_of_location(incident_lat, incident_long, 0.01);
    if(affected_edge == (unsigned) INFINITY)
    {
        response().make_error_response(404, "Incident location not on any road");
        return;
    }

    unsigned incident_id = incident_manager->add_incident(affected_edge, impact);

    /* Write route to response */
    cppcms::json::value response_body;
    response_body["data"] = incident_id;
    response().out() << response_body;
}

void IMSApp::remove_incident()
{
    /* Take URL parameter */
    string get_param = request().get("incident");
    if(get_param.empty())
    {
        response().make_error_response(400, "Empty incident ID");
        return;
    }

    unsigned incident_id;
    try
    {
        incident_id = stoi(get_param);
    }
    catch(exception e)
    {
        response().make_error_response(400, "Non-numerical parameter");
        return;
    }

    unsigned num_of_incident_removed = incident_manager->remove_incident(incident_id);
    if(num_of_incident_removed == 0)
    {
        response().make_error_response(404, "Incident not found");
        return;
    }

    response().status(200);

}

void IMSApp::handle_graph_update()
{
    response().out() << "handle_graph_update";
}
