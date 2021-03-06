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
#include <chrono>
#include <ctime>
#include <iomanip>

#include "ims/map_graph.h"
#include "ims/incident_manager.h"
#include "ims/router.h"
#include "experiment.h"

using namespace std;

int main()
{
    cout << "Start Experiment" << endl;

    float ss_long = 114.127758f;
    float ss_lat = 22.502621f;
    float hku_long = 114.135924f;
    float hku_lat = 22.283366f;

    float tp_long = 114.178089f;
    float tp_lat = 22.373222f;
    float st_long = 114.200127f;
    float st_lat = 22.390962;

    float tm_long = 113.969404f;
    float tm_lat = 22.395019f;
    float kt_long = 114.223126f;
    float kt_lat = 22.309550f;

    float tw_long = 114.113957f;
    float tw_lat = 22.371808f;
    float kc_long = 114.182248f;
    float kc_lat = 22.325167f;
    float mk_long = 114.168201f;
    float mk_lat = 22.322061f;


    float radius = 100;

    //experiment_all_route("ss_hku", ss_long, ss_lat, hku_long, hku_lat, radius);
    //experiment_all_route("tp_st", tp_long, tp_lat, st_long, st_lat, radius);
    //experiment_all_route("kt_tm", kt_long, kt_lat, tm_long, tm_lat, radius);
    //experiment_all_route("tw_mk", tw_long, tw_lat, mk_long, mk_lat, radius);
    experiment_all_route("tw_kc", tw_long, tw_lat, kc_long, kc_lat, radius);

    return 0;
}

void experiment_all_route(string suffix, float origin_long, float origin_lat, float destination_long, float destination_lat, float radius)
{
// prepare file
    ofstream fout;
    fout.open(suffix + "_data.csv");

    fout << "k-l,initial time,translate time,routing time,relase time,expanded nodes,expanded edge\n";
    fout << "1_2," << experiment_route(suffix + "_route_1_2", "HK_1_2.graph", origin_long, origin_lat, destination_long,destination_lat, radius);

    fout << "4-2," << experiment_route(suffix + "_route_4_2", "HK_4_2.graph", origin_long, origin_lat, destination_long,destination_lat, radius);
    fout << "8-2," << experiment_route(suffix + "_route_8_2", "HK_8_2.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "16-2," << experiment_route(suffix + "_route_16_2", "HK_16_2.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "32-2," << experiment_route(suffix + "_route_32_2", "HK_32_2.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "64-2," << experiment_route(suffix + "_route_64_2", "HK_64_2.graph", origin_long, origin_lat, destination_long, destination_lat, radius);

    fout << "4-3," << experiment_route(suffix + "_route_4_3", "HK_4_3.graph", origin_long, origin_lat, destination_long,destination_lat, radius);
    fout << "8-3," << experiment_route(suffix + "_route_8_3", "HK_8_3_new.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "16-3," << experiment_route(suffix + "_route_16_3", "HK_16_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "32-3," << experiment_route(suffix + "_route_32_3", "HK_32_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "64-3," << experiment_route(suffix + "_route_64_3", "HK_64_3.graph", origin_long, origin_lat, destination_long, destination_lat, radius);

    fout << "4-4," << experiment_route(suffix + "_route_4_4", "HK_4_4.graph", origin_long, origin_lat, destination_long,destination_lat, radius);
    fout << "8-4," << experiment_route(suffix + "_route_8_4", "HK_8_4_new.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "16-4," << experiment_route(suffix + "_route_16_4", "HK_16_4.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "32-4," << experiment_route(suffix + "_route_32_4", "HK_32_4.graph", origin_long, origin_lat, destination_long, destination_lat, radius);
    fout << "64-4," << experiment_route(suffix + "_route_64_4", "HK_64_4.graph", origin_long, origin_lat, destination_long, destination_lat, radius);

    fout << "8-5," << experiment_route(suffix + "_route_8_5", "HK_8_5_new.graph", origin_long, origin_lat, destination_long, destination_lat, radius);

    // close file
    fout.close();
}


string experiment_route(string filename, string graph, float origin_long, float origin_lat, float destination_long, float destination_lat, float radius)
{
    cout << "==== Experiment " << filename << " ====" << endl;
    ExpandedLog* log = new ExpandedLog();

    auto time_point_1 = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);

    /* Initializing system */
    cout << "Initializing MapGraph ..." << endl;
    auto map_graph = IMS::MapGraph::deserialize_and_initialize(graph);
    cout << "Initializing IncidentManager ..." << endl;
    auto incident_manager = new IMS::IncidentManager();
    cout << "Initializing Router ..." << endl;
    auto router = new IMS::Router(map_graph, incident_manager);

    auto time_point_2 = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);

    /* Translating data */
    cout << "Locating origin and destination ..." << endl;
    unsigned origin = map_graph->find_nearest_node_of_location(origin_long, origin_lat, radius);
    unsigned destination = map_graph->find_nearest_node_of_location(destination_long, destination_lat, radius);
    if(origin == RoutingKit::invalid_id)
    {
        cout << "No node within " << to_string(radius) << "m from origin position." << endl;
        return "No node within" + to_string(radius) + "m from origin position";
    }
    if(destination == RoutingKit::invalid_id)
    {
        cout << "No node within " << to_string(radius) << "m from destination position." << endl;
        return "No node within" + to_string(radius) + "m from destination position";
    }

    auto time_point_3 = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);

    /* Perform routing */
    cout << "Routing ..." << endl;
    time_t now = time(nullptr);
    IMS::Path * path = router->route(origin, destination, now, log);

    cout << "Completed." << endl;
    printf("Path: Time needed: %.2f minutes\n", (path->end_time - path->start_time) / 60000.0);
    cout << "Path: " << log->expanded_nodes.size() << " nodes and " << log->expanded_edges.size() << " edges expanded" << endl;

    cout << "Writing to XML ..." << endl;
    create_xml(filename, log);

    auto time_point_4 = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);
    auto no_node = log->expanded_nodes.size();
    auto no_edge = log->expanded_edges.size();

    /* Release memory */
    cout << "Releasing ..." << endl;
    delete path;
    delete log;
    delete router;
    delete incident_manager;
    delete map_graph;

    auto time_point_5 = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);

    cout << "Completed." << endl;

    /* Generate csv */
    string csv = "";
    csv = csv + to_string(time_point_2 - time_point_1) + ","; // initialize time
    csv = csv + to_string(time_point_3 - time_point_2) + ","; // translate time
    csv = csv + to_string(time_point_4 - time_point_3) + ","; // routing time
    csv = csv + to_string(time_point_5 - time_point_4) + ","; // release time
    csv = csv + to_string(no_node) + ","; // number of expanded nodes
    csv = csv + to_string(no_edge) + "\n"; // number of expanded edges
    return csv;
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
        unsigned id = node.first + 1;
        double lat = node.second.first;
        double lon = node.second.second;
        fout << "  <node id=\"" << id << "\" version=\"5\" timestamp=\"2017-08-31T16:06:25Z\" uid=\"0\" user=\"someone\" changeset=\"51620526\" lat=\"" << setprecision(7) << lat << "\" lon=\"" << setprecision(7) << lon << "\"/>" << endl;
    }

    // ways
    unsigned id_counter = 10001;
    for (auto edge : log->expanded_edges)
    {
        unsigned from_node = get<0>(edge) + 1;
        unsigned to_node = get<1>(edge) + 1;
        unsigned g = get<2>(edge);
        unsigned h = get<3>(edge);
        unsigned w = get<4>(edge);
        unsigned f = get<5>(edge);

        fout << "  <way id=\"" << id_counter << "\" version=\"1\" timestamp=\"2018-09-17T07:38:42Z\" uid=\"0\" user=\"someone\" changeset=\"62655145\">" << endl;
        fout << "    <nd ref=\"" << from_node << "\"/>" << endl;
        fout << "    <nd ref=\"" << to_node << "\"/>" << endl;
        fout << "    <tag k=\"highway\" v=\"secondary\"/>" << endl;
        fout << "    <tag k=\"route:g\" v=\"" << g << "\"/>" << endl;
        fout << "    <tag k=\"route:h\" v=\"" << h << "\"/>" << endl;
        fout << "    <tag k=\"route:w\" v=\"" << w << "\"/>" << endl;
        fout << "    <tag k=\"route:f\" v=\"" << f << "\"/>" << endl;

        fout << "    <tag k=\"partition_at_level:0\" v=\"" << get<6>(edge) << "\"/>" << endl;
        fout << "    <tag k=\"partition_at_level:1\" v=\"" << get<7>(edge) << "\"/>" << endl;
        fout << "    <tag k=\"partition_at_level:2\" v=\"" << get<8>(edge) << "\"/>" << endl;
        fout << "    <tag k=\"partition_at_level:3\" v=\"" << get<9>(edge) << "\"/>" << endl;
        fout << "    <tag k=\"partition_at_level:4\" v=\"" << get<10>(edge) << "\"/>" << endl;
        
        fout << "  </way>" << endl;
        id_counter++;
    }

    // final path
    id_counter = 10000001;
    for (auto edge : log->path_edges)
    {
        unsigned from_node = edge.first + 1;
        unsigned to_node = edge.second + 1;
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