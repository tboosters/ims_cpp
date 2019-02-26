#include <iostream>

#include "ims/map_graph.h"
#include "../src/partition.h"
#include "../src/routing.h"
#include "map_graph_test_data.h"

using namespace std;

int main()
{
    /* Prepare tests data */
    #define N 9
    #define hand 9
    #define small 12
    #define large 120

    vector<unsigned> nodes(N);
    for(unsigned i = 0; i < N; i++)
    {
        nodes[i] = i;
    }

    vector<float> longi(N);
    vector<float> lat(N);
    #if N == hand
    for(int i = 0; i < N; i++) {
        longi[i] = coordinates9[i][0];
        lat[i] = coordinates9[i][1];
    }
    #elif N == small
    for(int i = 0; i < N; i++) {
        longi[i] = coordinates12[i][0];
        lat[i] = coordinates12[i][1];
    }
    #elif N == large
    for(int i = 0; i < N; i++)
    {
        longi[i] = coordinates120[i][0];
        lat[i] = coordinates120[i][1];
    }
    #endif

    /* Driver */
    auto mapGraph = new IMS::MapGraph();
    mapGraph->latitude = lat;
    mapGraph->longitude = longi;
    #if N == hand
        mapGraph->first_out.assign(first_out9, first_out9 + N);
        mapGraph->head.assign(head9, head9 + 14);
        mapGraph->geo_distance.assign(geo_distance9, geo_distance9 + 14);
        mapGraph->default_travel_time.assign(default_travel_time9, default_travel_time9 + 14);
    #elif N == small
        mapGraph->first_out.assign(first_out12, first_out12 + N);
        mapGraph->head.assign(head12, head12 + N);
        mapGraph->geo_distance.assign(geo_distance12, geo_distance12 + N);
        mapGraph->default_travel_time.assign(default_travel_time12, default_travel_time12 + N);
    #elif N == large
        mapGraph->first_out.assign(first_out120, first_out120 + N);
        mapGraph->head.assign(head120, head120 + N);
        mapGraph->geo_distance.assign(geo_distance120, geo_distance120 + N);
        mapGraph->default_travel_time.assign(default_travel_time120, default_travel_time120 + N);
    #endif
    mapGraph->initialize();

    /* Graph tests */
    cout << "==== Graph Test ====" << endl;

    cout << "Original Graph:" << endl;
    mapGraph->print_graph();

    cout << "Inversed Graph:" << endl;
    mapGraph->inversed = mapGraph->inverse();
    mapGraph->print_inversed_graph();
    
    cout << endl;

    /* Partition + Layer tests */
    int k = 2;
    int l = 3;

    IMS::Partition::partition_t * p = IMS::Partition::do_partition(nodes, mapGraph, k, l, 0);
    IMS::Partition::index_partition(p);
    auto layer = IMS::Partition::build_layer(p, lat.size());

    IMS::Partition::print_partition(p);
    cout << endl << endl;

    IMS::Partition::print_layer(layer);
    cout << endl << endl;

    cout << IMS::Partition::find_parent(layer, 4, 2); // expected: 4 for N=12, k=2, l=3
    cout << endl;
    cout << IMS::Partition::find_parent(layer, 4); // expected: 4 for N=12, k=2, l=3

    IMS::Partition::delete_partition(p);

    /* Routing tests */

    // Find current density
    // Set density for edge 1 to be 0.1 (half of jam density) at time 100
    mapGraph->current_density[1][100] = 0.1;
    // Density of time >= 100 should be 0.1 and time < 100 should be 0
    assert(mapGraph->find_current_density(1, 99) == 0);
    assert(mapGraph->find_current_density(1, 100) == 0.1);

    // Weight retrieval function
    // Travel time needed before time = 100 should be shortest (default)
    assert(IMS::Routing::retrieve_weight(mapGraph, 1, 99) == mapGraph->default_travel_time[1]);
    assert(IMS::Routing::retrieve_weight(mapGraph, 1, 100) == 2 * (mapGraph->default_travel_time[1]));

    return 0;
}