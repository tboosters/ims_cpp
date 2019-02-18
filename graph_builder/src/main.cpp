/*
 * Driver program for the Graph Builder applet.
 * Provides user menu for graph building and serializing, and testing of created file.
 * Version: 1.0
 * Author: Terence Chow
 */

#include <iostream>

#include "build_mapgraph.h"
#include "test_serialized_file.h"

using namespace std;

const int BUILD_GRAPH_OPTION = 1;
const int TEST_FILE_OPTION = 2;
const int EXIT_OPTION = 3;


/*
 * Function for printing option menu of the app.
 */
void print_menu()
{
    cout << "╔══════════════════IMS Graph Builder═════════════════════════╗" << endl;
    cout << "║ 1: Preprocess MapGraph from PBF file and serialize to file ║" << endl;
    cout << "║ 2: Test serialized file                                    ║" << endl;
    cout << "║ 3: Exit                                                    ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    cout << "Selection: ";
}


int main()
{
    bool exit = false;
    int selection;

    do
    {
        /* Main UI loop */
        print_menu();
        cin >> selection;
        cout << endl;
        cin.ignore();           /* Clear input buffer */

        switch(selection)
        {
            case BUILD_GRAPH_OPTION:
                build_mapgraph_entrance();
                break;

            case TEST_FILE_OPTION:
                compare_serialized_graph();
                break;

            case EXIT_OPTION:
                exit = true;
                break;

            default:
                cout << "Invalid Input!" << endl;
                break;
        }
        cout << endl;

    } while(!exit);

    return 0;
}