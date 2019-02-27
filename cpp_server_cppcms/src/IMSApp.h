//
// Created by terchow on 12/5/18.
//

#ifndef PROJECT_IMSAPP_H
#define PROJECT_IMSAPP_H

#include <cppcms/application.h>
#include <cppcms/service.h>

#include "ims/map_graph.h"
#include "ims/incident_manager.h"

using namespace std;

namespace IMS
{

    class IMSApp : public cppcms::application
    {
    public:
        IMSApp(cppcms::service &srv, IMS::MapGraph *map_graph, IMS::IncidentManager *incident_manager);

    private:
        IMS::MapGraph *map_graph;
        IMS::IncidentManager *incident_manager;

        // Dev url for checking graph
        void check_graph();

        // Routed controller functions
        void route();
        void inject_incident();
        void remove_incident();
        void handle_graph_update();
    };

}


#endif //PROJECT_IMSAPP_H
