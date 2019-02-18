/*
 * Header file for map_graph module.
 * Version: 1.0
 * Author: Terence Chow
 */

#ifndef CPP_SERVER_MAPGRAPH_H
#define CPP_SERVER_MAPGRAPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

#include <routingkit/contraction_hierarchy.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

namespace IMS
{
    class MapGraph
    {
    public:
        /* TODO: Add fields and related functions - default_speed, current_travel_time, max_density, current_density */
        /* TODO: Add preprocessed data - layers, distance matrix */
        vector<float> latitude;
        vector<float> longitude;
        vector<unsigned> head;
        vector<unsigned> first_out;
        vector<unsigned> default_travel_time;
        vector<unsigned> geo_distance;


        /* Initialize from deserialization */
        static MapGraph deserialize(const string& input_file_path)
        {
            MapGraph graph;
            ifstream ifs(input_file_path);
            boost::archive::text_iarchive input_archive_stream(ifs);
            input_archive_stream >> graph;
            ifs.close();
            return graph;
        }

        /* Serialization*/
        void serialize(const string& output_file_path);

        /* Pre-processing */
        void partition(const int &k, const int &l);

        void preprocess();
    };

}

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
}
}
}


#endif  //CPP_SERVER_MAPGRAPH_H
