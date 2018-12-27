#include "utils.h"

#include <string>
#include <unistd.h>

using namespace std;

string get_current_dir()
{
    char buffer[FILENAME_MAX];
    getcwd(buffer, FILENAME_MAX);
    return string(buffer);
}