#include "utils.h"

#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

string get_exec_dir()
{
    char buffer[FILENAME_MAX];
    readlink("/proc/self/exe", buffer, FILENAME_MAX);
    string full_path(buffer);

    /* Get dir name */
    int pos = full_path.rfind('/');
    return full_path.substr(0, pos);
}