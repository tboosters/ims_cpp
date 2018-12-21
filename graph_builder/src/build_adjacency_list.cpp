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

#include "build_adjacency_list.h"

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
    graph.set_longitude(graph_data.longitude);
    graph.set_latitude(graph_data.latitude);
    graph.set_head(graph_data.head);
    graph.set_first_out(graph_data.first_out);
    graph.set_travel_time(graph_data.travel_time);
}

/*
 * Entrance of module from external source.
 * Acts as facet of graph building and serialization function.
 * Prompts user input for PBF file path.
 *
 * Parameters: NIL
 * Return: When graph building and serialization is done.
 */
void build_adjacency_list_entrance()
{
    string input_file_path; ///media/terchow/Data/U STUDY SOURCES/Year 4/FYP/hong_kong-latest.osm.pbf
    const string output_file_path = "HK.graph";

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
