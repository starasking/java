// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#ifndef GRAPH_SDK_DIRECTEDGRAPH_H
#define GRAPH_SDK_DIRECTEDGRAPH_H

#include <Eigen/Dense>
#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace graph_sdk {

struct EdgeCmp {
    bool operator()(const std::pair<size_t, size_t>& lhs,
                    const std::pair<size_t, size_t>& rhs) const {
        return std::max(lhs.first, lhs.second) <=
               std::max(rhs.first, rhs.second);
    }
};

struct WeightedEdgeCmp {
    bool operator()(const std::tuple<size_t, size_t, int>& lhs,
                    const std::tuple<size_t, size_t, int>& rhs) const {
        return std::max(std::get<0>(lhs), std::get<0>(lhs)) <=
               std::max(std::get<0>(rhs), std::get<0>(rhs));
    }
};
using Edges = std::set<std::pair<size_t, size_t>, EdgeCmp>;

struct pair_hash {
    template <class t1, class t2>
    size_t operator()(const std::pair<t1, t2>& pair) const {
        return std::hash<t1>()(pair.first) ^ std::hash<t2>()(pair.second);
    }
};

using WeightedEdges =
    std::unordered_map<std::pair<size_t, size_t>, int, pair_hash>;
// using WeightedEdges = std::set<std::tuple<size_t, size_t, int>,
// WeightedEdgeCmp>;
//  GeneralEdges = std::unordered_map<std::pair<size_t, size_t>, Edge>;

using GraphMatrix = Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>;
using Adjacency = std::vector<std::set<size_t>>;
enum class NodeAttribute { unlabelled, source, sink, source_sink, isolated };

struct Node {
    size_t id;
    int value;
};
struct Edge {
    size_t id;
    std::pair<size_t, size_t> arrow;
    int value;
};
class DirectedGraph {
   private:
    Adjacency adjacency_{};

    // helpers
   protected:
    void dfs_helper(size_t idx, std::vector<bool>& visited,
                    std::vector<size_t>& dfs_nodes) const;

    bool has_cycle_helper(size_t idx, std::vector<size_t>& visited) const;
    bool topo_sort_helper(size_t idx, std::stack<size_t>& visiting,
                          std::vector<size_t>& states) const;
    bool extract_scc_helper(size_t i, std::vector<size_t>& visited,
                            std::stack<size_t>& pseudo_topo,
                            std::vector<size_t>& scc) const;
    void extract_sc_helper(size_t curr, std::vector<size_t>& chain,
                           std::vector<int>& visited,
                           const std::vector<size_t>& scc,
                           std::vector<std::vector<size_t>>& cycles) const;
    std::vector<NodeAttribute> get_attribute() const;

   public:
    DirectedGraph() = default;
    explicit DirectedGraph(const Edges&);
    explicit DirectedGraph(const Adjacency&);

    // Basics
    void print_graph() const;
    void print_matrix() const;
    bool add_edge(std::pair<size_t, size_t> arrow);
    bool remove_node(size_t node);
    bool remove_edge(std::pair<size_t, size_t> arrow);

    GraphMatrix extract_matrix() const;
    Edges extract_edges() const;

    // Modify
    void reset();
    void exchange_nodes(size_t n1, size_t n2);

    // Random generate
    void random_generate(size_t V, size_t D);

    // Algorithm
    std::vector<size_t> dfs() const;
    std::pair<bool, std::stack<size_t>> topological_sort() const;
    bool has_cycle() const;
    std::pair<bool, std::vector<size_t>> extract_scc() const;
    DirectedGraph meta_graph() const;
    std::vector<std::vector<size_t>> extract_simple_cycles() const;
};

class DiWeightedGraph : public DirectedGraph {
   private:
    Adjacency adjacency_{};
    WeightedEdges weighted_edges_{};

   public:
    DiWeightedGraph() = default;
    explicit DiWeightedGraph(const WeightedEdges& edge);

    // Basics
    // void print_graph() const;
    // void print_matrix() const;
    bool add_edge(std::tuple<size_t, size_t, int> weighted_edge);
    bool remove_node(size_t node);
    bool remove_edge(std::pair<size_t, size_t> arrow);

    // GraphMatrix extract_matrix() const;
};

}  // namespace graph_sdk
#endif
