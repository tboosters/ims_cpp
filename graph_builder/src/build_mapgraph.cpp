/*
 * Module for option of building and serializing graph as adjacency list in C++ data structure
 * from OpenStreetMap PBF file.
 * Libraries: RoutingKit, Boost.Serialization
 * Version: 1.0
 * Author: Terence Chow
 */

#include <iostream>
#include <string>

#include <routingkit/osm_simple.h>
#include "ims/map_graph.h"

#include "build_mapgraph.h"

using namespace std;
using namespace RoutingKit;
using namespace IMS;

/*
 * Helper function for populating a MapGraph from a generate OSM graph.
 *
 * Parameters: IMS::MapGraph &graph
 *             const RoutingKit::SimpleOSMCarRoutingGraph &graph_data
 * Return: When MapGraph is populated;
 */
void populate_map_graph_from_osm_graph(IMS::MapGraph &graph, const RoutingKit::SimpleOSMCarRoutingGraph &graph_data)
{
    graph.longitude = graph_data.longitude;
    graph.latitude = graph_data.latitude;
    graph.head = graph_data.head;
    graph.first_out = graph_data.first_out;
    graph.default_travel_time = graph_data.travel_time;
    graph.geo_distance = graph_data.geo_distance;

    vector<unsigned> default_speed(graph_data.travel_time.size());
    for(unsigned i = 0; i < default_speed.size(); i++)
    {
        default_speed[i] = graph_data.geo_distance[i] / graph_data.travel_time[i];
    }
    graph.default_speed = default_speed;
}

/*
 * Entrance of module from external source.
 * Acts as facet of graph building and serialization function.
 * Prompts user input for PBF file path.
 *
 * Parameters: NIL
 * Return: When graph building and serialization is done.
 */
void build_mapgraph_entrance()
{
    string input_file_path;
    const string output_file_path = "HK.graph";

    int k, l;
    cout << "Number of Partitions (k): ";
    cin >> k;
    cout << "Number of Levels (l): ";
    cin >> l;

    cin.ignore();           /* Clear input buffer */
    cout << "PBF file path: ";
    getline(cin, input_file_path);

    try
    {
        /* Build graph data structure. */
        cout << "Start building graph..." << endl;

        RoutingKit::SimpleOSMCarRoutingGraph graph_data = simple_load_osm_car_routing_graph_from_pbf(input_file_path);
        IMS::MapGraph graph;
        populate_map_graph_from_osm_graph(graph, graph_data);

        cout << "Graph built." << endl;
        cout << "Number of nodes: " << graph_data.node_count() << endl;
        cout << "Number of edges: " << graph_data.arc_count() << endl;
        cout << "Tails of edges need to be created separately with invert_inverse_vetor(graph.first_out)" << endl;
        cout << endl;

        /* Perform Partitioning and Pre-processing */
        cout << "Partitioning..." << endl;
        graph.partition(k, l);

        cout << "Pre-processing..." << endl;
        graph.preprocess();

        /* Serialize created graph */
        cout << "Start serializing graph..." << endl;
        graph.serialize(output_file_path);
        cout << "Graph serialized and is stored at " << output_file_path << endl;
    }
    catch (runtime_error &e)
    {
        /* Catch exceptions thrown from graph building or serialization procedures */
        cout << e.what() << endl;
    }
}
