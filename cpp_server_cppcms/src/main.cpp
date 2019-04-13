#include <string>
#include <unistd.h>

#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <csignal>

#include "IMSApp.h"
#include "ims/map_graph.h"

using namespace std;

/* Utility function for finding excution path of this application instance.
 *
 * Parameter(s): NIL
 * Returns: string: Execution Path of the application instance
 */
string get_exec_dir()
{
    char buffer[FILENAME_MAX];
    readlink("/proc/self/exe", buffer, FILENAME_MAX);
    string full_path(buffer);

    /* Get dir name */
    int pos = full_path.rfind('/');
    return full_path.substr(0, pos);
}

/* Helper function to get MapGraph file path.
 *
 * Parameter(s): NIL
 * Returns: string: Path of MapGraph file.
 */
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

/* Driver function of the server.
 *
 * Parameter(s): NIL
 * Returns: when server is shut down.
 */
int main(int argc, char ** argv)
{
    string map_file_path = get_map_file_path(argc, argv);
    cout << "Using MapGraph file at: " << map_file_path << endl;
    try
    {
        cout << "Initializing MapGraph..." << endl;
        auto map_graph = IMS::MapGraph::deserialize_and_initialize(map_file_path);
        auto incident_manager = new IMS::IncidentManager();

        cppcms::service srv(argc, argv);
        srv.applications_pool().mount(cppcms::applications_factory<IMS::IMSApp>(map_graph, incident_manager));
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