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
#include "ims/router.h"


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

// Util function for building response body with IMS::Path
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

    printf("%f, %f\n%f, %f\n%d, %d", origin_long, origin_lat, destination_long, destination_lat, origin, destination);

    /* Perform routing */
    time_t now = time(nullptr);
    IMS::Path * path = router->route(origin, destination, now);

    /* Write route to response */
    cppcms::json::value response_body = build_path_response_body(path);
    response().out() << response_body;

    cout << endl << "==== Route ====" << endl;
    cout.precision(10);
    for(auto & n : path->nodes)
    {
        cout << n.second << ", " << n.first << endl;
    }
    printf("Time needed: %.2f minutes\n", (path->end_time - path->start_time) / 60.0);
    cout << "===============" << endl;

    /* Perform graph update */
    map_graph->inject_impact_of_routed_path(path);

    /* Release memory */
    delete path;
}

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
    old_path->start_time = (time_t) json_data["start_time"].number();
    old_path->end_time = (time_t) json_data["end_time"].number();

    for(auto & coordinates : json_data["nodes"].array())
    {
        old_path->nodes.emplace_back(coordinates[0].number(), coordinates[1].number());
    }

    for(auto & ete : json_data["enter_times"].object())
    {
        old_path->enter_times[stol(ete.first)] = (unsigned) ete.second.number();
    }

    /* Remove old path from graph's density information */
    map_graph->remove_impact_of_routed_path(old_path);

    /* Reroute from current location */
    unsigned destination = map_graph->find_nearest_node_of_location(destination_long, destination_lat, RADIUS);

    unsigned current_origin = map_graph->find_nearest_node_of_location(current_long, current_lat, RADIUS);
    if(current_origin == RoutingKit::invalid_id)
    {
        response().make_error_response(400, "No node within " + to_string(RADIUS) + "m from current position.");
        return;
    }

    time_t now = time(nullptr);
    auto new_path = router->route(current_origin, destination, now);

    /* Determine the better path: if new path reaches destination earlier than old path for at least "threshold" time */
    IMS::Path * better_path = old_path;
    time_t threshold = 10 * 60;
    if(new_path->end_time - old_path->end_time >= threshold)
    {
        better_path = new_path;
    }

    /* Write route to response */
    cppcms::json::value response_body = build_path_response_body(better_path);
    response().out() << response_body;

    cout << endl << "==== Route ====" << endl;
    for(auto & n : better_path->nodes)
    {
        cout << n.second << ", " << n.first << endl;
    }
    printf("Time needed: %.2f minutes\n", (better_path->end_time - better_path->start_time) / 60.0);
    cout << "===============" << endl;

    /* Perform graph update */
    map_graph->inject_impact_of_routed_path(better_path);

    /* Release memory */
    delete old_path;
    delete new_path;
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
