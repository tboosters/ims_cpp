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
#include <map>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

using namespace std;

namespace IMS
{
    struct Path
    {
        time_t start_time;
        time_t end_time;
        map<time_t, unsigned> enter_times; // <enter_time, edge ID>
        vector<double[2]> nodes; // [[long, lat]]
    };

    class MapGraph
    {
    public:
        /* TODO: Add preprocessed data - layers, distance matrix */
        /* TODO: Add Incident Management */

        vector<float> latitude;
        vector<float> longitude;
        vector<unsigned> head;
        vector<unsigned> first_out;
        vector<unsigned> geo_distance; // meter
        vector<unsigned> default_travel_time; // seconds
        vector<unsigned> default_speed; // m/s

        // Density related
        // current_density: vector id = edge ID, map key = critical change time, map value = density
        vector< map<time_t, double> > current_density;
        // Max Density = 1 / avg. car length = 1 / 5
        const double max_density = 0.2;

        /* Initialize dynamic fields: default_speed, current_density */
        void initialize()
        {
            default_speed.reserve(default_travel_time.size());
            for(unsigned i = 0; i < default_speed.size(); i++)
            {
                default_speed[i] = geo_distance[i] / default_travel_time[i];
            }

            current_density.reserve(default_travel_time.size());
            for (auto &current_density_map : current_density)
            {
                current_density_map[0] = 0;
            }
        }

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

        /* Routing */
        double find_current_density(unsigned edge, time_t enter_time);

        Path route(const double & origin_long, const double & origin_lat,
                const double & dest_long, const double & dest_lat, const time_t & start_time);
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
    archive & mapGraph.default_speed;

    /* Density Information */
    archive & mapGraph.current_density;
}
}
}


#endif  //CPP_SERVER_MAPGRAPH_H
