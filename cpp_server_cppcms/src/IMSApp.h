/*
 * Header file for IMSApp
 * Libraries: CPPCMS
 * Version: 1.0
 * Author: Terence Chow
 */

#ifndef PROJECT_IMSAPP_H
#define PROJECT_IMSAPP_H

#include <cppcms/application.h>
#include <cppcms/service.h>

#include "ims/map_graph.h"
#include "ims/incident_manager.h"
#include "ims/router.h"

using namespace std;

namespace IMS
{

    class IMSApp : public cppcms::application
    {
    public:
        IMSApp(cppcms::service &srv, IMS::MapGraph *map_graph, IMS::IncidentManager *incident_manager);

    private:
        static boost::mutex atomic_lock;

        IMS::MapGraph *map_graph;
        IMS::IncidentManager *incident_manager;
        IMS::Router *router;

        const float RADIUS = 100;
        const float OFFSET = 0.0008;

        // Dev url for checking graph
        void check_graph();

        // Routed controller functions
        void route();
        void reroute();
        void inject_incident();
        void remove_incident();
    };

}


#endif //PROJECT_IMSAPP_H
