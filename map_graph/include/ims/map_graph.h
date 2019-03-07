/*
 * Header file for map_graph module.
 * Version: 1.0
 * Author: Terence Chow & Yuen Hoi Man
 */

#ifndef CPP_SERVER_MAPGRAPH_H
#define CPP_SERVER_MAPGRAPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <routingkit/geo_position_to_node.h>

#include "../src/partition.h"
#include "../src/preprocess.h"

using namespace std;

namespace IMS
{
    struct Path
    {
        time_t start_time;
        time_t end_time;
        map<time_t, unsigned> enter_times; // <enter_time, edge ID>
        vector<pair<double, double>> nodes; // (long, lat)
    };

    struct InversedGraph
    {
        vector<unsigned> head;
        vector<unsigned> first_out;
        map<unsigned, unsigned> relative_edge;
    };

    class MapGraph
    {
    public:
        /* TODO: Add preprocessed data - layers, distance matrix */

        vector<float> latitude;
        vector<float> longitude;
        vector<unsigned> head;
        vector<unsigned> first_out;
        vector<unsigned> geo_distance; // meter
        vector<unsigned> default_travel_time; // seconds
        InversedGraph* inversed;
        RoutingKit::GeoPositionToNode map_geo_position; // Reversed geocoding index

        // Preprocessed data
        IMS::Partition::partition_t* partitions;
        IMS::Partition::layer_t* layers;
        IMS::Preprocess::distance_table_t* distance_tables;

        // Density related
        // current_density: vector id = edge ID, map key = critical change time, map value = density
        vector< map<time_t, double> > current_density;
        // Max Density = 1 / avg. car length = 1 / 5
        const double max_density = 0.2;

        /* Initialize dynamic fields: current_density, inversed, map_geo_location */
        void initialize();

        /* Initialize from deserialization */
        static MapGraph * deserialize(const string& input_file_path)
        {
            auto graph = new MapGraph();
            ifstream ifs(input_file_path);
            boost::archive::text_iarchive input_archive_stream(ifs);
            input_archive_stream >> *graph;
            ifs.close();
            return graph;
        }

        /* Serialization*/
        void serialize(const string& output_file_path);

        /* Inverse */
        InversedGraph* inverse();

        /* Pre-processing */
        void partition(const int &k, const int &l);
        void preprocess();

        /* Routing */
        unsigned find_edge(const unsigned &from, const unsigned &to);
        double find_current_density(unsigned edge, time_t enter_time);

        /* Updating */
        void inject_impact_of_routed_path(IMS::Path * path);
        void remove_impact_of_routed_path(IMS::Path * path);

        /* Reverse Geocoding */
        vector<unsigned int> find_nearest_edge_of_location(const float &longi, const float &lat, const float &offset);
        unsigned find_nearest_node_of_location(const float & longi, const float & lat, const float & radius);

        /* Util Functions */
        void print_graph();
        void print_inversed_graph();
    };

}

/* Schema for serialization of MapGraph in Boost.Serialization */
namespace boost
{
namespace serialization
{
template<class Archive>
void serialize(Archive &archive, IMS::MapGraph &mapGraph, const unsigned int version)
{
    /* Geo-location of nodes */
    archive & mapGraph.longitude;
    archive & mapGraph.latitude;

    /* Edges */
    /* Head node of each edge */
    archive & mapGraph.head;
    /* ID of the first of the batch of outward edges from i (i = Tail node of edge) */
    archive & mapGraph.first_out;

    /* Edge Information */
    archive & mapGraph.default_travel_time;
    archive & mapGraph.geo_distance;

    /* Preprocessed Data */
    archive & mapGraph.partitions;
    archive & mapGraph.layers;
    archive & mapGraph.distance_tables;
}
}
}


#endif  //CPP_SERVER_MAPGRAPH_H
