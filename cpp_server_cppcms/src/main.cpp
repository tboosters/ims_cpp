#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include "IMSApp.h"
#include "ims/map_graph.h"

using namespace std;

int main(int argc, char **argv)
{
    const string map_file_path = "./HK.graph";

    try
    {
        cout << "Initializing MapGraph..." << endl;
        IMS::MapGraph map_graph = IMS::MapGraph::deserialize(map_file_path);
        cppcms::service srv(argc, argv);
        srv.applications_pool().mount(cppcms::applications_factory<IMS::IMSApp>(&map_graph));
        cout << "Server starting at 8080..." << endl;
        srv.run();
    }
    catch (std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
    }
}