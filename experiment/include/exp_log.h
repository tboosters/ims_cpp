#ifndef EXP_LOG_H
#define EXP_LOG_H

#include <vector>
#include <map>

using namespace std;

class ExpandedLog
{
public:
    map<unsigned, pair<double, double>> expanded_nodes;
    //vector<tuple<unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, vector<pair<unsigned, unsigned>>>> expanded_edges;
    //vector<tuple<unsigned, unsigned, unsigned, unsigned, unsigned, unsigned>> expanded_edges;
    vector<tuple<unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned>> expanded_edges;

    vector<pair<unsigned, unsigned>> path_edges; 
};



#endif
