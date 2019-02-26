/*
 * Data structures of partition and layer. Stores layered partition information and parent of each node.
 * All structs and functions in namespace IMS::Partition.
 * Version: 1.0
 * Author: Terence Chow & Yuen Hoi Man
 */

#include <iostream>
#include <cmath>
#include <queue>
#include <unordered_set>

#include "partition.h"

using namespace std;
using namespace IMS::Partition;

/* Partition nodes into grids according to their coordinates. Empty grids are omitted.
 * Parameters: const vector<unsigned> & nodes
 *             const IMS::MapGraph * graph
 *             const int & k: number of rows / columns
 * Return: unordered_map<long, vector<unsigned> >: key: grid ID (disposable); value: list of nodes in the grid
 */
unordered_map<long, vector<unsigned> > IMS::Partition::grid_partition
        (const vector<unsigned> &nodes, const IMS::MapGraph * graph, const int &k)
{
    unordered_map<long, vector<unsigned> > node_partition;

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
        lat_max = graph->latitude[n] > lat_max? graph->latitude[n] : lat_max;
        lat_min = graph->latitude[n] < lat_min? graph->latitude[n] : lat_min;
        longi_max = graph->longitude[n] > longi_max? graph->longitude[n] : longi_max;
        longi_min = graph->longitude[n] < longi_min? graph->longitude[n] : longi_min;
    }

    double col_size = (longi_max - longi_min) / k;
    double row_size = (lat_max - lat_min) / k;

    for(const unsigned int & n : nodes)
    {
        // Assign the node in partitions in [a, a+1) range
        int col = (graph->longitude[n] - longi_min) / col_size;
        int row = (graph->latitude[n] - lat_min) / row_size;

        // Assign it to previous partition if the node is at the last boundary
        col = col == k? col-1 : col;
        row = row == k? row-1 : row;

        node_partition[row * k + col].push_back(n);
    }

    return node_partition;
}


/* Recursive function of partitioning
 * Parameters: const vector<unsigned> & nodes
 *             const IMS::MapGraph * graph
 *             const int & k
 *             const int & l
 *             const long & partition_id
 * Return: partition_t
 */
partition_t * IMS::Partition::do_partition
        (const vector<unsigned> &nodes, const IMS::MapGraph *graph, const int &k, const int &l,
         const unsigned &partition_id)
{
    auto p = new IMS::Partition::partition_t();
    p->id = partition_id;
    p->is_node = false;
    
    //Determine boundary nodes of the partition
    unordered_set<unsigned> node_set;
    for (unsigned node : nodes)
    {
        node_set.insert(node);
    }

    for (unsigned node : nodes)
    {
        unsigned first_edge = graph->first_out[node];
        unsigned last_edge = (node == graph->first_out.size() -1) ? (graph->head.size()) : graph->first_out[node + 1];
        for (unsigned edge = first_edge; edge < last_edge; edge++)
        {
            if (node_set.count(graph->head[edge]) > 0)
            {
                p->boundary_outwards.push_back(node);
                break;
            }
        }
    }

    if(l == 1)
    {
        vector<IMS::Partition::partition_t*> leaves(nodes.size());
        for(unsigned i = 0; i < nodes.size(); i++)
        {
            auto leaf = new IMS::Partition::partition_t;
            leaf->id = nodes[i];
            leaf->is_node = true;
            leaves[i] = leaf;
        }

        p->sub_partition = leaves;
        return p;
    }

    unordered_map<long, vector<unsigned> > sub_partitions = grid_partition(nodes, graph, k);
    long next_pid = 0;
    for (auto & sub_partition : sub_partitions) {
        partition_t * sp = do_partition(sub_partition.second, graph, k, l - 1, next_pid);

        p->sub_partition.push_back(sp);
        next_pid++;
    }

    return p;
}


/* Put unique numbering into partition_id of each level of the partition. Partition_id is unique only within the level.
 * Parameter: partition_t * p
 * Return: when indexing is done
 */
void IMS::Partition::index_partition(partition_t * p)
{
    unsigned pid = 0;
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
            curr->id = pid++;

            // Expand neighbours
            for(auto sp : curr->sub_partition)
            {
                if(!sp->is_node)
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

/* Build layer_t with partition information in partition_t layer-by-layer.
 * Parameter: partition_t * p
 *            const unsigned long & num_of_nodes
 * Return: layer_t
 * */
layer_t IMS::Partition::build_layer(IMS::Partition::partition_t *p, const unsigned long &num_of_nodes)
{
    layer_t layer(1);
    layer[0].push_back(INFINITY);

    int level_node_count = 1;
    int new_level_node_count = 0;
    vector<unsigned> nodes(num_of_nodes);
    queue<IMS::Partition::partition_t*> frontier;
    IMS::Partition::partition_t * curr;
    frontier.push(p);

    while(!frontier.empty())
    {
        vector<unsigned> next_level;
        for (int i = 0; i < level_node_count; i++)
        {
            // Get current node
            curr = frontier.front();
            frontier.pop();

            // Expand neighbour
            for (auto sp : curr->sub_partition)
            {
                if(sp->is_node)
                {
                    // Nodes
                    nodes[sp->id] = curr->id;
                }
                else
                {
                    // Partition
                    frontier.push(sp);
                    next_level.push_back(curr->id);
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

/* Print the partition structure. Each partition / node is in format of | partition_id (# of children) |.
 * Parameter: partition_t * p
 * Return: when partition is printed
 */
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
            cout << curr->id << " ";
            cout << "(" << curr->sub_partition.size() << ") ";
            cout << "(";
            for(int i = 0; i < curr->boundary_outwards.size(); i++)
            {
                cout << curr->boundary_outwards[i];
                cout << (i == curr->boundary_outwards.size()-1? "" : ", ");
            }
            cout << ") | ";

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

/* Print layer structure.
 * Parameters: const IMS::Partition::layer_t & layer
 * Return: when layer is printed
 */
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


/* Retrieves parent partition of a node at specified level.
 * Parameters: const IMS::Partition::layer_t & layer
 *             const long & node
 *             const long & level: optional, omit to get immediate parent
 * Return: long: partition_id of parent
 */
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

/* Recursively releases memory allocated to a partition.
 * Parameters: IMS::Partition::partition_t *& p:
 *                     Pointer p must be passed by reference such that the value of the pointer itself can be changed
 * Return: when partition memory is released.
 */
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