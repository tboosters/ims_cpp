#include <iostream>
#include <cassert>

#include "map_graph_test_data.h"
#include "../include/ims/router.h"

using namespace std;

int main()
{
    auto map_graph = new IMS::MapGraph();
    for(int i = 0; i < 4; i++) {
        map_graph->longitude.push_back(coordinates4[i][0]);
        map_graph->latitude.push_back(coordinates4[i][1]);
    }
    map_graph->first_out.assign(first_out4, first_out4 + 4);
    map_graph->head.assign(head4, head4 + 5);
    map_graph->geo_distance.assign(geo_distance4, geo_distance4 + 5);
    map_graph->default_travel_time.assign(default_travel_time4, default_travel_time4 + 5);
    map_graph->initialize();
    map_graph->current_density[1][100] = 0.1;
    map_graph->partition(2, 3);
    map_graph->preprocess();

    auto incident_manager = new IMS::IncidentManager();
    auto router = new IMS::Router(map_graph, incident_manager);

    //cout << router->retrieve_future_weight(0, 3) << endl;

    cout << "==== Router Test ====" << endl;
    // Weight retrieval function
    // Travel time needed before time = 100 should be shortest (default)
    assert(router->retrieve_realized_weight(1, 99) == map_graph->default_travel_time[1]);
    assert(router->retrieve_realized_weight(1, 100) == 2 * (map_graph->default_travel_time[1]));

    cout << "==== All Router Test passed ====" << endl;
}
