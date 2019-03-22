/*
 * Experiment
 * Version: 1.0
 * Author: Yuen Hoi Man
 */

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "ims/map_graph.h"
#include "ims/incident_manager.h"
#include "ims/router.h"
#include "experiment.h"

using namespace std;

int main()
{
    //float origin_long = 114.127758f;
    //float origin_lat = 22.502621f;
    float origin_long = 114.178089f;
    float origin_lat = 22.373222f;
    float destination_long = 114.135924f;
    float destination_lat = 22.283366f;
    float radius = 100;

    experiment_route("route_4_3", "HK_4_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    experiment_route("route_8_3", "HK_4_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    experiment_route("route_16_3", "HK_4_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    experiment_route("route_32_3", "HK_4_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    experiment_route("route_64_3", "HK_4_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);


    return 0;
}

void experiment_route(string filename, string graph, float origin_long, float origin_lat, float destination_long, float destination_lat, float radius)
{
    cout << "==== Experiment ====" << endl;
    ExpandedLog* log = new ExpandedLog();

    cout << "Initializing MapGraph ..." << endl;
    auto map_graph = IMS::MapGraph::deserialize_and_initialize(graph);
    cout << "Initializing IncidentManager ..." << endl;
    auto incident_manager = new IMS::IncidentManager();
    cout << "Initializing Router ..." << endl;
    auto router = new IMS::Router(map_graph, incident_manager);

    cout << "Locating origin and destination ..." << endl;
    unsigned origin = map_graph->find_nearest_node_of_location(origin_long, origin_lat, radius);
    unsigned destination = map_graph->find_nearest_node_of_location(destination_long, destination_lat, radius);
    if(origin == RoutingKit::invalid_id)
    {
        cout << "No node within " << to_string(100) << "m from origin position." << endl;
        return;
    }
    if(destination == RoutingKit::invalid_id)
    {
        cout << "No node within " << to_string(100) << "m from destination position." << endl;
        return;
    }

    /* Perform routing */
    cout << "Routing ..." << endl;
    time_t now = time(nullptr);
    IMS::Path * path = router->route(origin, destination, now, log);

    cout << "Completed." << endl;
    printf("Path: Time needed: %.2f minutes\n", (path->end_time - path->start_time) / 60000.0);
    cout << "Path: " << log->expanded_nodes.size() << " nodes and " << log->expanded_edges.size() << " edges expanded" << endl;

    cout << "Writing to XML ..." << endl;
    create_xml(filename, log);

    /* Release memory */
    cout << "Releasing ..." << endl;
    delete path;
    delete log;
    delete router;
    delete incident_manager;

    cout << "Completed." << endl;

}

void create_xml(string filename, ExpandedLog* log) // <from_node, to_node>
{
    // prepare file
    ofstream fout;
    fout.open(filename + ".xml");

    // osm xml header
    fout << "<?xml version='1.0' encoding='UTF-8'?>" << endl;
    fout << "<osm version=\"0.6\" generator=\"Osmosis 0.5\">" << endl;

    // bound
    fout << "   <bounds minlon=\"113.813\" minlat=\"22.133\" maxlon=\"114.506\" maxlat=\"22.572\" origin=\"CGImap 0.6.1 (2000 thorn-03.openstreetmap.org)\"/>" << endl;

    // nodes
    for (auto node : log->expanded_nodes)
    {
        unsigned id = node.first;
        double lat = node.second.first;
        double lon = node.second.second;
        fout << "  <node id=\"" << id << "\" version=\"5\" timestamp=\"2017-08-31T16:06:25Z\" uid=\"0\" user=\"someone\" changeset=\"51620526\" lat=\"" << setprecision(7) << lat << "\" lon=\"" << setprecision(7) << lon << "\"/>" << endl;
    }

    // ways
    unsigned id_counter = 10001;
    for (auto edge : log->expanded_edges)
    {
        unsigned from_node = get<0>(edge);
        unsigned to_node = get<1>(edge);
        unsigned g = get<2>(edge);
        unsigned h = get<3>(edge);
        unsigned w = get<4>(edge);
        fout << "  <way id=\"" << id_counter << "\" version=\"1\" timestamp=\"2018-09-17T07:38:42Z\" uid=\"0\" user=\"someone\" changeset=\"62655145\">" << endl;
        fout << "    <nd ref=\"" << from_node << "\"/>" << endl;
        fout << "    <nd ref=\"" << to_node << "\"/>" << endl;
        fout << "    <tag k=\"highway\" v=\"secondary\"/>" << endl;
        fout << "    <tag k=\"g\" v=\"" << g << "\"/>" << endl;
        fout << "    <tag k=\"h\" v=\"" << h << "\"/>" << endl;
        fout << "    <tag k=\"w\" v=\"" << w << "\"/>" << endl;
        fout << "  </way>" << endl;
        id_counter++;
    }

    // final path
    id_counter = 10000001;
    for (auto edge : log->path_edges)
    {
        unsigned from_node = edge.first;
        unsigned to_node = edge.second;
        fout << "  <way id=\"" << id_counter << "\" version=\"1\" timestamp=\"2018-09-17T07:38:42Z\" uid=\"0\" user=\"someone\" changeset=\"62655145\">" << endl;
        fout << "    <nd ref=\"" << from_node << "\"/>" << endl;
        fout << "    <nd ref=\"" << to_node << "\"/>" << endl;
        fout << "    <tag k=\"highway\" v=\"primary\"/>" << endl;
        fout << "  </way>" << endl;
        id_counter++;
    }

    // close osm xml header
    fout << "</osm>" << endl;

    // close file
    fout.close();
}