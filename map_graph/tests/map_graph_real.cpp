#include <map>
#include <unordered_set>

#include "../src/partition.h"
#include "ims/map_graph.h"

using namespace std;

int main()
{
    auto graph = IMS::MapGraph::deserialize("HK.graph");
    graph->initialize();

    vector<unsigned> nodes(graph->latitude.size());
    for(unsigned i = 0; i < graph->latitude.size(); i++)
    {
        nodes[i] = i;
    }

    int k = 8; // k = 8
    int lvl = 5; // lvl = 5
    IMS::Partition::partition_t * p = IMS::Partition::do_partition(nodes, graph->latitude, graph->longitude,
            graph->head, graph->first_out, graph->inversed->head, graph->inversed->first_out, k, lvl, 0);
    IMS::Partition::index_partition(p);
    IMS::Partition::layer_t* l = IMS::Partition::build_layer(p, nodes.size());

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
}