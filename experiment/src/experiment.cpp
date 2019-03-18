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

#include "ims/map_graph.h"
#include "experiment.h"

using namespace std;

int main()
{
    cout << "Experiment!" << endl;
    ExpandedLog* log = new ExpandedLog();
    
    
    
    create_xml("somename", log);
    cout << "Created file" << endl;
    return 0;
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
    fout << "   <bounds minlon=\"114.11260\" minlat=\"22.49460\" maxlon=\"114.14070\" maxlat=\"22.51120\" origin=\"CGImap 0.6.1 (2000 thorn-03.openstreetmap.org)\"/>" << endl;

    // nodes
    for (auto node : log->expanded_nodes)
    {
        unsigned id = node.first;
        unsigned lat = node.second.first;
        unsigned lon = node.second.second;
        fout << "  <node id=\"" << id << "\" version=\"5\" timestamp=\"2017-08-31T16:06:25Z\" uid=\"0\" user=\"someone\" changeset=\"51620526\" lat=\"" << lat << "\" lon=\"" << lon << "\"/>" << endl;
    }

    // ways
    unsigned id_counter = 0;
    for (auto edge : log->expanded_edges)
    {
        unsigned from_node = edge.first;
        unsigned to_node = edge.second;
        fout << "  <way id=\"" << id_counter << "\" version=\"1\" timestamp=\"2018-09-17T07:38:42Z\" uid=\"0\" user=\"someone\" changeset=\"62655145\">" << endl;
        fout << "    <nd ref=\"" << from_node << "\"/>" << endl;
        fout << "    <nd ref=\"" << to_node << "\"/>" << endl;
        fout << "  </way>" << endl;
        id_counter++;
    }

    // close osm xml header
    fout << "</osm>" << endl;

    // close file
    fout.close();
}