#include <map>
#include <unordered_set>

#include <chrono>
#include <ctime>

#include "../src/partition.h"
#include "ims/map_graph.h"

using namespace std;

int main()
{
    auto start_time = chrono::system_clock::now();
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "Start program @ \t" << ctime(&now);

    auto graph = IMS::MapGraph::deserialize("HK.graph");
    graph->initialize();

    return 0; 

    /*
    vector<unsigned> nodes(graph->latitude.size());
    for(unsigned i = 0; i < graph->latitude.size(); i++)
    {
        nodes[i] = i;
    }
    */

    int k = 8; // k = 8
    int lvl = 5; // lvl = 5

    now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "Data preparation completed @ \t" << ctime(&now);
    cout << "Start partitioning @ \t" << ctime(&now);

    /*
    IMS::Partition::partition_t * p = IMS::Partition::do_partition(nodes, graph->latitude, graph->longitude,
            graph->head, graph->first_out, graph->inversed->head, graph->inversed->first_out, k, lvl, 0);
    IMS::Partition::index_partition(p);
    IMS::Partition::layer_t* l = IMS::Partition::build_layer(p, nodes.size());
    */
   /*
    graph->partition(k, lvl);

    now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "Partitioning completed @ \t" << ctime(&now);
    cout << "Start preprocessing @ \t" << ctime(&now);

    graph->preprocess();

    now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "Preprocessing completed @ \t" << ctime(&now);
    */
    
    auto end_time = chrono::system_clock::now();
    auto elapsed_seconds = end_time - start_time;

    cout << "Elapsed time: " << elapsed_seconds.count() << "s." << endl;

    /*
    while(1)
    {
        float lat, longi, offset;
        cin >> longi >> lat >> offset;
        vector<unsigned> affected_edges = graph->find_nearest_edge_of_location(longi, lat, offset);
        for(auto & edge : affected_edges)
        {
            cout << edge << " ";
        }
        cout << endl;
    }
    */
}