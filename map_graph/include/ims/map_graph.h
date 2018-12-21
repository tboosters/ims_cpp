#ifndef CPP_SERVER_PISTACHE_MAPGRAPH_H
#define CPP_SERVER_PISTACHE_MAPGRAPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

namespace IMS
{

    class MapGraph
    {
    private:
        vector<float> latitude;
        vector<float> longitude;
        vector<unsigned> head;
        vector<unsigned> first_out;
        vector<unsigned> travel_time;

        /* Function overload for Boost.Serialization, specifying the serialization scheme */
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &archive, const unsigned int version)
        {
            /* Geo-location of nodes */
            archive & longitude;
            archive & latitude;

            /* Edges */
            /* Head node of each edge */
            archive & head;
            /* ID of the first of the batch of outward edges from i (i = Tail node of edge) */
            archive & first_out;

            /* Edge Information */
            archive & travel_time;
        }

    public:
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
        void serialize(const string& output_file_path)
        {
            ofstream ofs(output_file_path);
            boost::archive::text_oarchive output_archive_stream(ofs);
            output_archive_stream << *this;
            ofs.close();
        }

        /* Getters */
        const vector<float> &get_latitude() const;

        const vector<float> &get_longitude() const;

        const vector<unsigned int> &get_head() const;

        const vector<unsigned int> &get_first_out() const;

        const vector<unsigned int> &get_travel_time() const;

        /* Setters */
        void set_latitude(const vector<float> &latitude);

        void set_longitude(const vector<float> &longitude);

        void set_head(const vector<unsigned int> &head);

        void set_first_out(const vector<unsigned int> &first_out);

        void set_travel_time(const vector<unsigned int> &travel_time);

    };

}

#endif
