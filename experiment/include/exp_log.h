#ifndef EXP_LOG_H
#define EXP_LOG_H

#include <vector>
#include <map>

using namespace std;

class ExpandedLog
{
public:
    map<unsigned, pair<unsigned, unsigned>> expanded_nodes;
    vector<pair<unsigned, unsigned>> expanded_edges;
};



#endif
