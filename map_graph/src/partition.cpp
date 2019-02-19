/*
 * Data structures of partition and layer. Stores layered partition information and parent of each node.
 * All structs and functions in namespace IMS::Partition.
 * Version: 1.0
 * Author: Terence Chow
 */

#include <iostream>
#include <cmath>
#include <queue>

#include "partition.h"

using namespace std;
using namespace IMS::Partition;

unordered_map<long, vector<unsigned int> > IMS::Partition::grid_partition
        (const vector<unsigned int> &nodes, const vector<float> &latitude,
                                       const vector<float> &longitude, const int &k)
{
    unordered_map<long, vector<unsigned int> > node_partition;

    if(nodes.size() == 1)
    {
        node_partition[nodes[0]].push_back(nodes[0]);
        return node_partition;
    }

    float lat_max = -INFINITY;
    float lat_min = INFINITY;
    float longi_max = -INFINITY;
    float longi_min = INFINITY;

    for(const unsigned & n : nodes)
    {
        lat_max = latitude[n] > lat_max? latitude[n] : lat_max;
        lat_min = latitude[n] < lat_min? latitude[n] : lat_min;
        longi_max = longitude[n] > longi_max? longitude[n] : longi_max;
        longi_min = longitude[n] < longi_min? longitude[n] : longi_min;
    }

    double col_size = (longi_max - longi_min) / k;
    double row_size = (lat_max - lat_min) / k;

    for(const unsigned int & n : nodes)
    {
        // Assign the node in partitions in [a, a+1) range
        int col = (longitude[n] - longi_min) / col_size;
        int row = (latitude[n] - lat_min) / row_size;

        // Assign it to previous partition if the node is at the last boundary
        col = col == k? col-1 : col;
        row = row == k? row-1 : row;

        node_partition[row * k + col].push_back(n);
    }

    return node_partition;
}


partition_t * IMS::Partition::do_partition
        (const vector<unsigned int> &nodes, const vector<float> &latitude,
                                                  const vector<float> &longitude, const int &k, const int &l,
                                                  const long &partition_id)
{
    auto p = new IMS::Partition::partition_t;
    p->partition_id = partition_id;
    p->node_id = -1;

    if(l == 1)
    {
        vector<IMS::Partition::partition_t*> leaves(nodes.size());
        for(unsigned int i = 0; i < nodes.size(); i++)
        {
            auto leaf = new IMS::Partition::partition_t;
            leaf->partition_id = nodes[i];
            leaf->node_id = nodes[i];
            leaves[i] = leaf;
        }

        p->sub_partition = leaves;
        return p;
    }

    unordered_map<long, vector<unsigned int> > sub_partitions = grid_partition(nodes, latitude, longitude, k);
    long next_pid = 0;
    for (auto & sub_partition : sub_partitions) {
        partition_t * sp = do_partition(sub_partition.second, latitude, longitude, k, l - 1, next_pid);
        p->sub_partition.push_back(sp);
        next_pid++;
    }

    return p;
}


void IMS::Partition::index_partition(partition_t * p)
{
    long pid = 0;
    int level_node_count = 1;
    int new_level_node_count = 0;
    queue<partition_t*> frontier;
    IMS::Partition::partition_t * curr;
    frontier.push(p);

    while(!frontier.empty())
    {
        for(int i = 0; i < level_node_count; i++)
        {
            // Get current node
            curr = frontier.front();
            frontier.pop();

            // Do stuff with current node
            curr->partition_id = pid++;

            // Expand neighbours
            for(auto sp : curr->sub_partition)
            {
                if(sp->node_id == -1)
                {
                    frontier.push(sp);
                }
            }
            new_level_node_count += curr->sub_partition.size();
        }
        level_node_count = new_level_node_count;
        new_level_node_count = 0;
        pid = 0;
    }
}

/*
 * Layer: Records parent of each node / partition in the partition hierarchy
 * e.g. layer[x][y] = Partition ID of parent of a node / partition y in level x
 * */
layer_t IMS::Partition::build_layer(IMS::Partition::partition_t *p, const unsigned long &num_of_nodes)
{
    layer_t layer(1);
    layer[0].push_back(-1);

    int level_node_count = 1;
    int new_level_node_count = 0;
    vector<long> nodes(num_of_nodes);
    queue<IMS::Partition::partition_t*> frontier;
    IMS::Partition::partition_t * curr;
    frontier.push(p);

    while(!frontier.empty())
    {
        vector<long> next_level;
        for (int i = 0; i < level_node_count; i++)
        {
            // Get current node
            curr = frontier.front();
            frontier.pop();

            // Expand neighbour
            for (auto sp : curr->sub_partition)
            {
                if(sp->node_id == -1)
                {
                    // Partition
                    frontier.push(sp);
                    next_level.push_back(curr->partition_id);
                }
                else
                {
                    // Nodes
                    nodes[sp->node_id] = curr->partition_id;
                }
            }
            new_level_node_count += curr->sub_partition.size();
        }

        if(!frontier.empty())
        {
            layer.push_back(next_level);
        }
        level_node_count = new_level_node_count;
        new_level_node_count = 0;
    }
    layer.push_back(nodes);

    return layer;
}

void IMS::Partition::print_partition(IMS::Partition::partition_t * p)
{
    int level_node_count = 1;
    int new_level_node_count = 0;
    queue<IMS::Partition::partition_t*> frontier;
    IMS::Partition::partition_t * curr;
    frontier.push(p);

    while(!frontier.empty())
    {
        for(int i = 0; i < level_node_count; i++)
        {
            // Get current node
            curr = frontier.front();
            frontier.pop();

            // Do stuff with current node
            if(curr->node_id == -1)
            {
                cout << curr->partition_id << " ";
            }
            else
            {
                cout << curr->node_id << " ";
            }
            cout << "(" << curr->sub_partition.size() << ") | ";

            // Expand neighbour
            for(auto sp : curr->sub_partition)
            {
                frontier.push(sp);
            }
            new_level_node_count += curr->sub_partition.size();
        }
        cout << endl;
        level_node_count = new_level_node_count;
        new_level_node_count = 0;
    }
}

void IMS::Partition::print_layer(const IMS::Partition::layer_t & layer)
{
    for(const auto &l : layer)
    {
        for(auto p : l)
        {
            cout << p << " | ";
        }
        cout << endl;
    }
}

/* Optional level parameter for immediate parent */
long IMS::Partition::find_parent(const IMS::Partition::layer_t &layer, const long &node, const long &level/* = -1 */)
{
    long parent = node;

    if(level == -1)
    {
        parent = layer[layer.size()-1][node];
    }
    else
    {
        for(unsigned long i = layer.size()-1; i > level; i--)
        {
            parent = layer[i][parent];
        }
    }

    return parent;
}

void IMS::Partition::delete_partition(IMS::Partition::partition_t *& p)
{
    if(p != nullptr)
    {
        for(auto sp : p->sub_partition)
        {
            delete_partition(sp);
        }

        delete p;
        p = nullptr;
    }
}