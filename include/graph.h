// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#ifndef GRAPH_SDK_DIRECTEDGRAPH_H
#define GRAPH_SDK_DIRECTEDGRAPH_H

#include <algorithm>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../include/matrix.h"

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
    size_t VN_{};
    size_t EN_{};

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
                           std::vector<bool>& valid,
                           const std::vector<size_t>& scc,
                           std::vector<std::vector<size_t>>& cycles) const;
    std::vector<NodeAttribute> get_attribute() const;

   public:
    DirectedGraph() = default;
    explicit DirectedGraph(const size_t N);
    explicit DirectedGraph(const Edges&);
    explicit DirectedGraph(const Adjacency& adjacency)
        : adjacency_(adjacency) {}
    explicit DirectedGraph(const Matrix<size_t>& matrix);

    // Basics
    void print_graph() const;
    void print_matrix() const;
    bool add_edge(std::pair<size_t, size_t> arrow);
    bool remove_node(size_t node);
    bool remove_edge(std::pair<size_t, size_t> arrow);
    bool random_remove_edges(size_t n);

    Matrix<size_t> extract_matrix() const;
    Matrix<int> extract_di_matrix() const;
    Edges extract_edges() const;
    size_t calculate_edge_num() const;
    size_t fetch_edge_num() const;
    DirectedGraph reverse_graph() const;

    // Modify
    void reset();
    void exchange_nodes(size_t n1, size_t n2);

    // Random generate
    void random_generate(size_t V, size_t D);
    void random_generate_dag(size_t V, size_t D);
    DirectedGraph generate_bipartite_dag() const;
    DirectedGraph graph_shuffle() const;

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
    bool add_edge(std::tuple<size_t, size_t, int> weighted_edge);
    bool remove_node(size_t node);
    bool remove_edge(std::pair<size_t, size_t> arrow);
};

}  // namespace graph_sdk
#endif
