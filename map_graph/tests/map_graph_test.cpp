#include <iostream>
#include <cmath>
#include <algorithm>

#include "../include/ims/map_graph.h"
#include "../src/partition.h"
#include "../src/preprocess.h"
#include "map_graph_test_data.h"

using namespace std;

int main()
{
    /* Prepare tests data */
    #define N 16
    #define square 4
    #define hand 9
    #define hand2 16

    vector<unsigned> nodes(N);
    for(unsigned i = 0; i < N; i++)
    {
        nodes[i] = i;
    }

    vector<float> longi(N);
    vector<float> lat(N);
    #if N == square
    for(int i = 0; i < N; i++) {
        longi[i] = coordinates4[i][0];
        lat[i] = coordinates4[i][1];
    }
    #elif N == hand
    for(int i = 0; i < N; i++) {
        longi[i] = coordinates9[i][0];
        lat[i] = coordinates9[i][1];
    }
    #elif N == hand2
    for (int i = 0; i < N; i++) {
        longi[i] = coordinates16[i][0];
        lat[i] = coordinates16[i][1];
    }
    #endif

    /* Driver */
    auto mapGraph = new IMS::MapGraph();
    mapGraph->latitude = lat;
    mapGraph->longitude = longi;
    #if N == square
        mapGraph->first_out.assign(first_out4, first_out4 + N);
        mapGraph->head.assign(head4, head4 + 5);
        mapGraph->geo_distance.assign(geo_distance4, geo_distance4 + 5);
        mapGraph->default_travel_time.assign(default_travel_time4, default_travel_time4 + 5);
    #elif N == hand
        mapGraph->first_out.assign(first_out9, first_out9 + N);
        mapGraph->head.assign(head9, head9 + 14);
        mapGraph->geo_distance.assign(geo_distance9, geo_distance9 + 14);
        mapGraph->default_travel_time.assign(default_travel_time9, default_travel_time9 + 14);
    #elif N == hand2
        mapGraph->first_out.assign(first_out16, first_out16 + N);
        mapGraph->head.assign(head16, head16 + 30);
        mapGraph->geo_distance.assign(geo_distance16, geo_distance16 + 30);
        mapGraph->default_travel_time.assign(default_travel_time16, default_travel_time16 + 30);
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

    #define STREAMLINE  1
    #if STREAMLINE == 0
    cout << "==== Partition & Layer Test ====" << endl;
    
    IMS::Partition::partition_t * p = IMS::Partition::do_partition(nodes, mapGraph->latitude, mapGraph->longitude,
            mapGraph->head, mapGraph->first_out, mapGraph->inversed->head, mapGraph->inversed->first_out, k, l, 0);
    IMS::Partition::index_partition(p);
    auto layer = IMS::Partition::build_layer(p, lat.size());

    cout << "Partition:" << endl;
    IMS::Partition::print_partition(p);
    cout << endl << endl;

    cout << "Layer:" << endl;
    IMS::Partition::print_layer(layer);
    cout << endl;

    cout << IMS::Partition::find_parent(layer, 4, 2); // expected: 4 for N=12, k=2, l=3
    cout << endl;
    cout << IMS::Partition::find_parent(layer, 4); // expected: 4 for N=12, k=2, l=3
    cout << endl;

    //IMS::Partition::delete_partition(p);

    /* Preprocess tests */
    cout << "==== Preprocess Test ====" << endl;
    IMS::Preprocess::distance_table_t* distance_table = IMS::Preprocess::do_preprocess(nodes, mapGraph->head, 
            mapGraph->first_out, mapGraph->default_travel_time, p, &layer);

    cout << "Distance table:" << endl;
    IMS::Preprocess::print_distance_table(distance_table);
    cout << endl;

    #else
    cout << "==== Partition & Layer Test ====" << endl;
    mapGraph->partition(k, l);

    cout << "Partition:" << endl;
    IMS::Partition::print_partition(mapGraph->partitions);
    cout << endl << endl;

    cout << "Layer:" << endl;
    IMS::Partition::print_layer(mapGraph->layers);
    cout << endl;

    //IMS::Partition::delete_partition(p);

    /* Preprocess tests */
    cout << "==== Preprocess Test ====" << endl;
    mapGraph->preprocess();
    cout << "Distance table:" << endl;
    IMS::Preprocess::print_distance_table(mapGraph->distance_tables);
    cout << endl;
    #endif

    /* Test graph for Reverse Geocoding and Update Tests */
    auto mapGraph_square = new IMS::MapGraph();
    for(int i = 0; i < 4; i++) {
        mapGraph_square->longitude.push_back(coordinates4[i][0]);
        mapGraph_square->latitude.push_back(coordinates4[i][1]);
    }
    mapGraph_square->first_out.assign(first_out4, first_out4 + 4);
    mapGraph_square->head.assign(head4, head4 + 5);
    mapGraph_square->geo_distance.assign(geo_distance4, geo_distance4 + 5);
    mapGraph_square->default_travel_time.assign(default_travel_time4, default_travel_time4 + 5);
    mapGraph_square->initialize();

    /* Routing tests */
    cout << "==== Reverse Geocoding Test ====" << endl;
    // Find current density
    // Set density for edge 1 to be 0.1 (half of jam density) at time 100
    mapGraph_square->current_density[1][100] = 0.1;
    // Density of time >= 100 should be 0.1 and time < 100 should be 0
    assert(mapGraph_square->find_current_density(1, 99) == 0);
    assert(mapGraph_square->find_current_density(1, 100) == 0.1);
    mapGraph_square->current_density[1].erase(100);


    // Find nearest edge with location tests, longi = x, lat = y
    assert(mapGraph_square->find_nearest_edge_of_location(0, 0.5, 0)[0] == 0);
    assert(mapGraph_square->find_nearest_edge_of_location(-0.5, 0.5, 0).empty());
    assert(mapGraph_square->find_nearest_edge_of_location(-0.5, 0.5, 0.5)[0] == 0);
    assert(mapGraph_square->find_nearest_edge_of_location(0.5, 0.7, 0).empty());
    assert(mapGraph_square->find_nearest_edge_of_location(0.5, 0.7, 0.2)[0] == 4);
    assert(mapGraph_square->find_nearest_edge_of_location(0, 0, 0)[0] == 0);

    // Test for multiple affected edges
    vector<unsigned> affected_edges = mapGraph_square->find_nearest_edge_of_location(0.1, 0.1, 0.1);
    assert(affected_edges.size() == 3);
    assert(find(affected_edges.begin(), affected_edges.end(), 0) != affected_edges.end());
    assert(find(affected_edges.begin(), affected_edges.end(), 1) != affected_edges.end());
    assert(find(affected_edges.begin(), affected_edges.end(), 4) != affected_edges.end());

    cout << "==== All Reverse Geocoding Test passed ====" << endl;
    cout << endl;

    /* Graph Update Tests */
    // Create Path
    auto path1 = new IMS::Path();
    path1->start_time = 60;
    path1->end_time = 120;
    path1->enter_times[60] = 0;
    path1->enter_times[90] = 1;
    path1->enter_times[100] = 3;

    auto pathEarly0 = new IMS::Path();
    pathEarly0->start_time = 50;
    pathEarly0->end_time = 70;
    pathEarly0->enter_times[50] = 0;

    auto pathLate1 = new IMS::Path();
    pathLate1->start_time = 95;
    pathLate1->end_time = 110;
    pathLate1->enter_times[95] = 1;

    auto pathSame3 = new IMS::Path();
    pathSame3->start_time = 100;
    pathSame3->end_time = 120;
    pathSame3->enter_times[100] = 3;

    cout << "==== Graph Update Test ====" << endl;

    // Case: Empty density cache, i.e. no previous vehicles
    mapGraph_square->update_with_routed_path(path1);
    // Path1 - edge 0
    assert(mapGraph_square->current_density[0].size() == 3);
    assert(mapGraph_square->current_density[0][0] == 0);
    assert(mapGraph_square->current_density[0][60] == 1.0 / mapGraph_square->geo_distance[0]);
    assert(mapGraph_square->current_density[0][90] == 0);
    // Path1 - edge 1
    assert(mapGraph_square->current_density[1].size() == 3);
    assert(mapGraph_square->current_density[1][0] == 0);
    assert(mapGraph_square->current_density[1][90] == 1.0 / mapGraph_square->geo_distance[1]);
    assert(mapGraph_square->current_density[1][100] == 0);
    // Path1 - edge 3
    assert(mapGraph_square->current_density[3].size() == 3);
    assert(mapGraph_square->current_density[3][0] == 0);
    assert(mapGraph_square->current_density[3][100] == 1.0 / mapGraph_square->geo_distance[3]);
    assert(mapGraph_square->current_density[3][120] == 0);

    // Case: Another vehicle comes earlier and leaves earlier.
    // Note: Impossible for other vehicles to come earlier and leave later.
    mapGraph_square->update_with_routed_path(pathEarly0);
    assert(mapGraph_square->current_density[0].size() == 5);
    assert(mapGraph_square->current_density[0][0] == 0);
    assert(mapGraph_square->current_density[0][50] == 1.0 / mapGraph_square->geo_distance[0]);
    assert(mapGraph_square->current_density[0][60] == 2.0 / mapGraph_square->geo_distance[0]);
    assert(mapGraph_square->current_density[0][70] == 1.0 / mapGraph_square->geo_distance[0]);
    assert(mapGraph_square->current_density[0][90] == 0);

    // Case: Another vehicle comes later and leaves later
    // Note: Impossible for other vehicles to come later and leave earlier.
    mapGraph_square->update_with_routed_path(pathLate1);
    assert(mapGraph_square->current_density[1].size() == 5);
    assert(mapGraph_square->current_density[1][0] == 0);
    assert(mapGraph_square->current_density[1][90] == 1.0 / mapGraph_square->geo_distance[1]);
    assert(mapGraph_square->current_density[1][95] == 2.0 / mapGraph_square->geo_distance[1]);
    assert(mapGraph_square->current_density[1][100] == 1.0 / mapGraph_square->geo_distance[1]);
    assert(mapGraph_square->current_density[1][110] == 0);

    // Case: Another vehicle comes and leaves at existing critical time
    mapGraph_square->update_with_routed_path(pathSame3);
    assert(mapGraph_square->current_density[3].size() == 3);
    assert(mapGraph_square->current_density[3][0] == 0);
    assert(mapGraph_square->current_density[3][100] == 2.0 / mapGraph_square->geo_distance[3]);
    assert(mapGraph_square->current_density[3][120] == 0);

    cout << "==== All Graph Update Test passed ====" << endl;

    return 0;
}