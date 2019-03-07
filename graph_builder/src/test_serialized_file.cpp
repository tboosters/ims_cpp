/*
 * Module for option of testing created serialization of graph data structure.
 * Deserializes the "HK.graph" file and compares it with a newly converted graph.
 * Libraries: RoutingKit, Boost.Serialization
 * Version: 1.0
 * Author: Terence Chow
 */

#include <iostream>
#include <fstream>
#include <string>

#include <routingkit/osm_simple.h>
#include "ims/map_graph.h"

#include "test_serialized_file.h"

using namespace std;
using namespace RoutingKit;
using namespace IMS;

/*
 * Entrance point of module from external source.
 * Prompts for paths of serialized file (actual result) and
 * original PBF file (expected result).
 *
 * Parameters: NIL
 * Return: When graph deserialization and comparisons are done.
 */
void compare_serialized_graph()
{
    string actual_file_path;        //HK.graph;
    string original_pbf_path;       ///media/terchow/Data/U STUDY SOURCES/Year 4/FYP/hong_kong-latest.osm.pbf

    /*
     * Read user input for file paths
     */
    cout << "Serialized file path: ";
    cin >> actual_file_path;
    cin.ignore();                        /* Clear input buffer */
    cout << "Original PBF file path: ";
    getline(cin, original_pbf_path);
    cout << endl;

    /*
     * Deserialize file and create graph for comparison
     */
    try
    {
        cout << "Deserializing actual HK.graph..." << endl;
        auto actual = IMS::MapGraph::deserialize_and_initialize(actual_file_path);

        cout << "Creating expected graph..." << endl;
        RoutingKit::SimpleOSMCarRoutingGraph expected = simple_load_osm_car_routing_graph_from_pbf(original_pbf_path);

        /*
         * Output results
         */
        cout << "Comparing results..." << endl;
        cout << "Node Count | Actual: " << actual->latitude.size() << " | Expected: " << expected.node_count() << endl;
        cout << "Arc Count | Actual: " << actual->head.size() << " | Expected: " << expected.arc_count() << endl;
        cout << "Head | " << (actual->head == expected.head? "Pass" : "Fail") << endl;
        cout << "First Out | " << (actual->first_out == expected.first_out? "Pass" : "Fail") << endl;
        cout << "Latitude and Longitude | " <<
                (actual->latitude == expected.latitude
                    && actual->longitude == expected.longitude? "Pass" : "Fail") << endl;
        cout << "Travel time | " << (actual->default_travel_time == expected.travel_time? "Pass" : "Fail") << endl;
        cout << "Geo Distance | " << (actual->geo_distance == expected.geo_distance? "Pass" : "Fail") << endl;
    }
    catch (runtime_error &e)
    {
        /* Catch exceptions thrown from graph building or serialization procedures */
        cout << e.what() << endl;
    };
}