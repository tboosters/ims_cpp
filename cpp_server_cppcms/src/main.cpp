#include <string>

#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include "utils.h"
#include "IMSApp.h"
#include "ims/map_graph.h"

using namespace std;

/* Helper function to get MapGraph file path */
string get_map_file_path(const int & argc, char ** argv) {
    if(argc > 3)
    {
        if(strcmp(argv[3], "-m") == 0)
        {
            return string(argv[4]); /* Get user defined file path. */
        }
        else
        {
            /* Invalid argument, exit */
            cout << "Usage: " << argv[0] << " -c <config.js> [-m <map file path>]" << endl;
            exit(1);
        }
    }
    else
    {
        /* Use default MapGraph file dir: executable directory */
        const string map_file_dir = get_exec_dir();
        const string map_file_name = "HK.graph";
        return map_file_dir + "/" + map_file_name;
    }
}

/* Driver function of the server */
int main(int argc, char ** argv)
{
    string map_file_path = get_map_file_path(argc, argv);
    cout << "Using MapGraph file at: " << map_file_path << endl;
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
        return 1;
    }

    return 0;
}