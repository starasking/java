// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#ifndef GRAPH_SDK_DIRECTEDGRAPH_H
#define GRAPH_SDK_DIRECTEDGRAPH_H

#include <Eigen/Dense>
#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <tuple>
#include <vector>

namespace graph_sdk {

struct EdgeCmp {
    bool operator()(const std::pair<size_t, size_t>& lhs,
                    const std::pair<size_t, size_t>& rhs) const {
        return std::max(lhs.first, lhs.second) <=
               std::max(rhs.first, rhs.second);
    }
};
using Edges = std::set<std::pair<size_t, size_t>, EdgeCmp>;
// Edges = std::map<std::pair<size_t, size_t>, Edge>;

using GraphMatrix = Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>;
using Adjacency = std::vector<std::set<size_t>>;

class DirectedGraph {
   private:
    struct Edge {
        size_t id;
        std::pair<size_t, size_t> arrow;
        int value;
    };
    struct Node {
        size_t id;
        int value;
    };

    enum class NodeAttribute {
        unlabelled,
        source,
        sink,
        source_sink,
        isolated
    };

    Adjacency adjacency_{};

    // helpers
    void dfs_helper(size_t idx, std::vector<bool>& visited,
                    std::vector<size_t>& dfs_nodes) const;

    bool has_cycle_helper(size_t idx, std::vector<size_t>& visited) const;
    bool topo_sort_helper(size_t idx, std::stack<size_t>& visiting,
                          std::vector<size_t>& states) const;
    bool extract_scc_helper(size_t i, std::vector<size_t>& visited,
                            std::stack<size_t>& pseudo_topo,
                            std::vector<size_t>& scc) const;
    void extract_sc_helper(size_t curr, std::vector<size_t>& chain,
                           std::vector<bool>& visited, std::vector<size_t>& scc,
                           std::vector<std::vector<size_t>>& cycles) const;
    std::vector<NodeAttribute> get_attribute() const;

   public:
    DirectedGraph() = default;
    DirectedGraph(const Edges&);
    DirectedGraph(const Adjacency&);

    void print_graph() const;
    void print_matrix() const;
    bool add_edge(std::pair<size_t, size_t> arrow);
    bool remove_node(size_t node);
    bool remove_edge(std::pair<size_t, size_t> arrow);

    GraphMatrix extract_matrix() const;
    Edges extract_edges() const;

    // Modify
    //
    void reset();
    void exchange_nodes(size_t n1, size_t n2);

    // Algorithm
    std::vector<size_t> dfs() const;
    std::pair<bool, std::stack<size_t>> topological_sort() const;
    bool has_cycle() const;
    std::pair<bool, std::vector<size_t>> extract_scc() const;
    DirectedGraph meta_graph() const;
    std::vector<std::vector<size_t>> extract_simple_cycles() const;

    // Random generate
    void random_generate(size_t V, size_t D);

    /*
    // logs
    greedy-related
    minimum_spanning_tree: electrical grid, computer network
    shortest_path: greedy

    longest_path
    tic_toc_toe
    graph_in_molecule
    bipartite
    articulation points
    max-flow min-cut
    graph-colouring
    planer graphs (?)
    union-find
    matching
    */
};

/*
class DirectedWeightedGraph: class DirectedGraph
{
    private:
        Adjacency adjacency{};
        std::map<std::pair<int, int>, int> edge_weight_table{};
    public:
        DirectedWeightedGraph(const WeightedEdges&);

        bool add_edge(int from_idx, int to_idx, int value);
        Matrix extract_matrix();
};
*/
}  // namespace graph_sdk
#endif
