/*
 * Backend Server Application. Serves web API endpoints for routing and incident management.
 * Keeps reference to MapGraph, IncidentManager and Router.
 * Libraries: CPPCMS
 * Version: 1.0
 * Author: Terence Chow
 */

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

#include "IMSApp.h"
#include "ims/map_graph.h"
#include "ims/incident_manager.h"
#include "ims/router.h"


using namespace std;
using namespace IMS;

/* Utility function for extracting JSON data from raw request body.
 *
 * Parameter(s): pair<void *,size_t> raw_post_data
 * Returns: cppcms::json::value: extracted JSON data
 */
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

/* Utility function for building JSON object to be returned for a found Path.
 *
 * Parameter(s): const IMS::Path *path
 * Returns: cppcms::json::value: built path in JSON
 */
cppcms::json::value build_path_response_body(const IMS::Path *path)
{
    cppcms::json::value response_body;

    response_body["data"]["start_time"] = path->start_time;
    response_body["data"]["end_time"] = path->end_time;

    for(int i = 0; i < path->nodes.size(); i++)
    {
        response_body["data"]["nodes"][i][0] = path->nodes[i].first;
        response_body["data"]["nodes"][i][1] = path->nodes[i].second;
    }

    for(auto & ete : path->enter_times)
    {
        response_body["data"]["enter_times"][to_string(ete.first)] = ete.second;
    }
    return response_body;
}

// Initialize static atomic_lock
boost::mutex IMSApp::atomic_lock;


/* Constructor of IMSApp class.
 * Initializes essential class fields. Assign handler functions to corresponding URL endpoints.
 *
 * Parameter(s): const IMS::Path *path
 * Returns: cppcms::json::value: built path in JSON
 */
IMSApp::IMSApp(cppcms::service &srv, IMS::MapGraph *map_graph, IMS::IncidentManager *incident_manager) : cppcms::application(srv)
{
    this->map_graph = map_graph;
    this->incident_manager = incident_manager;
    this->router = new IMS::Router(map_graph, incident_manager);

    // Dev url for checking graph
    dispatcher().map("GET", "/graph", &IMSApp::check_graph, this);

    // Add url dispatchers
    dispatcher().map("POST", "/route", &IMSApp::route, this);
    dispatcher().map("POST", "/reroute", &IMSApp::reroute, this);
    dispatcher().map("POST", "/incident", &IMSApp::inject_incident, this);
    dispatcher().map("DELETE", "/incident", &IMSApp::remove_incident, this);
}

/* Dev-only endpoint for checking basic information of loaded MapGraph.
 *
 * Parameter(s): NIL
 * Returns: when request is done.
 */
void IMSApp::check_graph()
{
    response().out() << "Node Count: " << map_graph->latitude.size();
    response().out() << "<br>";
    response().out() << "Edge Count: " << map_graph->head.size();
}

/* Handler function for POST /route.
 * Takes request body -> Finds nearest nodes in MapGraph -> Route -> Return path -> Update.
 * Requires atomic_lock before entering Route and Update.
 *
 * Parameter(s): JSON object with format:
 * {
 *   "coordinates": [[longitude, latitude], [longitude, latitude]]
 * }
 * Returns: Found path to requester, error on no nodes / path found.
 */
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
    unsigned origin = map_graph->find_nearest_node_of_location(origin_long, origin_lat, RADIUS);
    unsigned destination = map_graph->find_nearest_node_of_location(destination_long, destination_lat, RADIUS);
    if(origin == RoutingKit::invalid_id)
    {
        response().make_error_response(400, "No node within " + to_string(RADIUS) + "m from origin position.");
        return;
    }
    if(destination == RoutingKit::invalid_id)
    {
        response().make_error_response(400, "No node within " + to_string(RADIUS) + "m from destination position.");
        return;
    }

    /* Perform routing */
    boost::lock_guard<boost::mutex> lock(IMS::IMSApp::atomic_lock);

    time_t now = time(nullptr);
    IMS::Path * path = router->route(origin, destination, now);
    /* Perform graph update */

    if(path != nullptr)
    {
        map_graph->inject_impact_of_routed_path(path);

        /* Write route to response */
        cppcms::json::value response_body = build_path_response_body(path);
        response().out() << response_body;

        cout << endl << "==== Route ====" << endl;
        cout.precision(10);
        for(auto & n : path->nodes)
        {
            cout << n.second << ", " << n.first << endl;
        }
        printf("Time needed: %.2f minutes\n", (path->end_time - path->start_time) / 60000.0);
        cout << "===============" << endl;

        /* Release memory */
        delete path;
    }
    else
    {
        printf("NULL PATH: %f, %f - %f, %f\n", origin_lat, origin_long, destination_lat, destination_long);
        response().make_error_response(400, "Path not found.");
    }
}

/* Handler function for POST /reroute.
 * Takes request body -> Remove path from Current Density -> Finds nearest nodes in MapGraph -> Route -> Return path -> Update.
 * Requires atomic_lock before entering Route and Update.
 *
 * Parameter(s): JSON object with format:
 * {
 *   "coordinates": [[longitude, latitude], [longitude, latitude]],
 *   <original path>
 * }
 * Returns: Found path to requester, error on no nodes / path found.
 */
void IMSApp::reroute()
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

    double current_long = json_data["coordinates"][0][0].number();
    double current_lat = json_data["coordinates"][0][1].number();
    double destination_long = json_data["coordinates"][1][0].number();
    double destination_lat = json_data["coordinates"][1][1].number();

    auto old_path = new IMS::Path();
    old_path->start_time = (time_t) json_data["path"]["start_time"].number();
    old_path->end_time = (time_t) json_data["path"]["end_time"].number();

    for(auto & coordinates : json_data["path"]["nodes"].array())
    {
        old_path->nodes.emplace_back(coordinates[0].number(), coordinates[1].number());
    }

    for(auto & ete : json_data["path"]["enter_times"].object())
    {
        old_path->enter_times[stol(ete.first)] = (unsigned) ete.second.number();
    }

    /* Reroute from current location */
    unsigned destination = map_graph->find_nearest_node_of_location(destination_long, destination_lat, RADIUS);

    unsigned current_origin = map_graph->find_nearest_node_of_location(current_long, current_lat, RADIUS);
    if(current_origin == RoutingKit::invalid_id)
    {
        response().make_error_response(400, "No node within " + to_string(RADIUS) + "m from current position.");
        return;
    }

    boost::lock_guard<boost::mutex> lock(IMS::IMSApp::atomic_lock);
    /* Remove old path from graph's density information */
    map_graph->remove_impact_of_routed_path(old_path);

    time_t now = time(nullptr);
    auto new_path = router->route(current_origin, destination, now);

    /* Perform graph update */
    if(new_path != nullptr)
    {
        map_graph->inject_impact_of_routed_path(new_path);

        /* Write route to response */
        cppcms::json::value response_body = build_path_response_body(new_path);
        response().out() << response_body;

        cout << endl << "==== Route ====" << endl;
        for(auto & n : new_path->nodes)
        {
            cout << n.second << ", " << n.first << endl;
        }
        printf("Time needed: %.2f minutes\n", (new_path->end_time - new_path->start_time) / 60000.0);
        cout << "===============" << endl;

        /* Release memory */
        delete old_path;
        delete new_path;
    }
    else
    {
        printf("NULL PATH: %f, %f - %f, %f\n", current_lat, current_long, destination_lat, destination_long);
        response().make_error_response(400, "Path not found.");
    }
}

/* Handler function for POST /incident.
 * Takes request body -> Finds affected edges in MapGraph -> Inject incident -> Return incident ID.
 *
 * Parameter(s): JSON object with format:
 * {
 *   "location": [longitude, latitude],
 *   "impact": impact on travelling time in milliseconds
 * }
 * Returns: incident ID,  error on no edge found.
 */
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
    /* OFFSET OF NEAREST_EDGE = 0.002 for accuracy of result */
    vector<unsigned> affected_edges = map_graph->find_nearest_edge_of_location(incident_long, incident_lat, OFFSET);
    if(affected_edges.empty())
    {
        response().make_error_response(400, "Incident location not on any road");
        return;
    }

    unsigned incident_id = incident_manager->add_incident(affected_edges, impact);

    /* Write route to response */
    cppcms::json::value response_body;
    response_body["data"] = incident_id;
    response().out() << response_body;
}


/* Handler function for DELETE /incident.
 * Reads request parameter -> Removes incidnet -> Return deletion status.
 *
 * Request URL: DELETE /incident?incident=<incident ID>
 *
 * Returns: success status code,  error on no incident found.
 */
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
        response().make_error_response(400, "Incident not found");
        return;
    }

    response().status(200);
}
