#include <map>
#include <unordered_set>

#include "../src/partition.h"
#include "ims/map_graph.h"

using namespace std;

int main()
{
    auto graph = IMS::MapGraph::deserialize("HK.graph");
    vector<unsigned> nodes(graph.latitude.size());
    for(unsigned i = 0; i < graph.latitude.size(); i++)
    {
        nodes[i] = i;
    }

    int k = 8; // k = 8
    int lvl = 5; // lvl = 5
    IMS::Partition::partition_t * p = IMS::Partition::do_partition(nodes, &graph, k, lvl, 0);
    IMS::Partition::index_partition(p);
    IMS::Partition::layer_t l = IMS::Partition::build_layer(p, nodes.size());

    map<unsigned, vector<unsigned> > pl;
    for(unsigned i = 0; i < nodes.size(); i++)
    {
        pl[l[lvl][i]].push_back(i);
    }
}