// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stack>
#include <unordered_map>
#include <utility>

#include "../include/graph.h"
#include "../include/utils.h"

namespace graph_sdk {

// generation
DirectedGraph::DirectedGraph(const Edges& edges) {
    assert(adjacency_.empty());

    auto node = *edges.rbegin();
    adjacency_.resize(std::max(node.first, node.second) + 1);

    std::for_each(edges.begin(), edges.end(),
                  [&](const auto& x) { adjacency_[x.first].insert(x.second); });
}

DirectedGraph::DirectedGraph(const Adjacency& input) {
    assert(adjacency_.empty());
    adjacency_ = input;
}

void DirectedGraph::random_generate(size_t V, size_t D) {
    assert(adjacency_.empty());
    srand(time(NULL));  // use current time as seed for random generator
    adjacency_.assign(V, std::set<size_t>{});
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) continue;
            if (rand() % V < D) {
                adjacency_[i].insert(j);
            }
        }
    }
}
// modification
bool DirectedGraph::add_edge(std::pair<size_t, size_t> arrow) {
    if (auto max_tmp = std::max(arrow.first, arrow.second);
        max_tmp >= adjacency_.size()) {
        adjacency_.resize(max_tmp + 1);
    }
    auto tmp = adjacency_[arrow.first].insert(arrow.second);
    return tmp.second;
}

bool DirectedGraph::remove_node(size_t idx) {
    auto N = adjacency_.size();
    if (idx >= N) return false;

    if (idx == N - 1) {
        adjacency_.resize(idx);
    } else if (!adjacency_[idx].empty()) {
        adjacency_[idx] = {};
    }

    std::for_each(adjacency_.begin(), adjacency_.end(), [&](auto& x) {
        if (auto it = x.find(idx); it != x.end()) x.erase(it);
    });

    return true;
}

bool DirectedGraph::remove_edge(std::pair<size_t, size_t> arrow) {
    if (arrow.first >= adjacency_.size()) return false;
    if (auto it = adjacency_[arrow.first].find(arrow.second);
        it != adjacency_[arrow.first].end()) {
        adjacency_[arrow.first].erase(it);
        return true;
    } else
        return false;
}

void DirectedGraph::exchange_nodes(size_t n1, size_t n2) {
    assert(std::max(n1, n2) < adjacency_.size());
    std::swap(adjacency_[n1], adjacency_[n2]);

    for (auto& x : adjacency_) {
        if (x.empty()) continue;
        auto iter1 = x.find(n1);
        auto iter2 = x.find(n2);
        if (iter1 != x.end() && iter2 == x.end()) {
            x.erase(iter1);
            x.insert(n2);
        } else if (iter2 != x.end() && iter1 == x.end()) {
            x.erase(iter2);
            x.insert(n1);
        }
    }
}

void DirectedGraph::reset() {
    std::vector<NodeAttribute> attributes = DirectedGraph::get_attribute();
    std::set<size_t> missed{};
    auto N = adjacency_.size();

    for (size_t i = 0; i < N; ++i) {
        if (attributes[i] == NodeAttribute::isolated) missed.insert(i);
    }

    auto it = missed.begin();
    std::unordered_map<size_t, size_t> exchange_ids{};
    size_t i = 0;
    while (it != missed.end()) {
        if (missed.find(N - 1 - i) != missed.end()) ++i;
        if (*it < N - 1 - i) {
            exchange_ids.insert({N - 1 - i, *it});
        }
        ++i;
        ++it;
    }

    for (auto [key, value] : exchange_ids) {
        std::swap(adjacency_[key], adjacency_[value]);
    }
    adjacency_.resize(adjacency_.size() - missed.size());

    for (auto& x : adjacency_) {
        if (x.empty()) continue;
        for (auto it = x.begin(); it != x.end();) {
            if (auto iter = exchange_ids.find(*it);
                iter != exchange_ids.end()) {
                it = x.erase(it);
                x.insert(iter->second);
            } else
                ++it;
        }
    }
}

// representation
GraphMatrix DirectedGraph::extract_matrix() const {
    auto N = adjacency_.size();
    GraphMatrix matrix(N, N);
    matrix.setZero();
    for (size_t i = 0; i < N; ++i) {
        for (const auto& x : adjacency_[i]) {
            matrix(i, x) = 1;  // TODO: for weighted?
        }
    }
    return matrix;
}

Edges DirectedGraph::extract_edges() const {
    Edges edges;
    for (size_t i = 0; i < adjacency_.size(); ++i) {
        for (const auto& y : adjacency_[i]) edges.insert({i, y});
    }
    return edges;
}

std::vector<NodeAttribute> DirectedGraph::get_attribute() const {
    std::vector<NodeAttribute> attribute(adjacency_.size(),
                                         NodeAttribute::unlabelled);
    for (auto i = 0; i < adjacency_.size(); ++i) {
        if (adjacency_.empty()) {
            if (attribute[i] == NodeAttribute::unlabelled)
                attribute[i] = NodeAttribute::isolated;
        } else {
            if (attribute[i] == NodeAttribute::unlabelled)
                attribute[i] = NodeAttribute::source;
            std::for_each(
                adjacency_[i].begin(), adjacency_[i].end(), [&](const auto& x) {
                    attribute[x] = (attribute[x] == NodeAttribute::source ||
                                    attribute[x] == NodeAttribute::source_sink)
                                       ? NodeAttribute::source_sink
                                       : NodeAttribute::sink;
                });
        }
    }

    return attribute;
}
// depth first search related algorithms
void DirectedGraph::dfs_helper(size_t idx, std::vector<bool>& visited,
                               std::vector<size_t>& dfs_nodes) const {
    visited[idx] = true;
    dfs_nodes.push_back(idx);

    for (const auto x : adjacency_[idx])
        if (!visited[x]) DirectedGraph::dfs_helper(x, visited, dfs_nodes);
}

std::vector<size_t> DirectedGraph::dfs() const {
    auto N = adjacency_.size();
    std::vector<bool> visited(N, false);
    std::vector<size_t> dfs_nodes{};

    for (auto i = 0; i < N; ++i) {
        if (!visited[i]) DirectedGraph::dfs_helper(i, visited, dfs_nodes);
    }
    return dfs_nodes;
}

bool DirectedGraph::topo_sort_helper(size_t i, std::stack<size_t>& visiting,
                                     std::vector<size_t>& visited) const {
    visited[i] = 1;
    for (const auto x : adjacency_[i]) {
        if (visited[x] == 1 ||
            (visited[x] == 0 &&
             !DirectedGraph::topo_sort_helper(x, visiting, visited)))
            return false;
    }
    visited[i] = 2;
    visiting.push(i);
    return true;
}

std::pair<bool, std::stack<size_t>> DirectedGraph::topological_sort() const {
    const auto N = adjacency_.size();
    std::stack<size_t> visiting{};
    std::vector<size_t> visited(N, 0);
    for (auto i = 0; i < N; ++i) {
        if (visited[i] == 0 &&
            !DirectedGraph::topo_sort_helper(i, visiting, visited))
            return std::make_pair(false, std::stack<size_t>{});
    }
    return std::make_pair(true, visiting);
}

bool DirectedGraph::has_cycle_helper(size_t idx,
                                     std::vector<size_t>& visited) const {
    // 1 for visited, 2 for done-visited
    visited[idx] = 1;
    for (const auto x : adjacency_[idx]) {
        if (visited[x] == 1 ||
            (visited[x] == 0 && DirectedGraph::has_cycle_helper(x, visited)))
            return true;
    }
    visited[idx] = 2;
    return false;
}

bool DirectedGraph::has_cycle() const {
    std::vector<size_t> visited(adjacency_.size(), 0);
    for (size_t i = 0; i < adjacency_.size(); ++i) {
        if (visited[i] == 0 && DirectedGraph::has_cycle_helper(i, visited))
            return true;
    }
    return false;
}

bool DirectedGraph::extract_scc_helper(size_t i, std::vector<size_t>& visited,
                                       std::stack<size_t>& pseudo_topo,
                                       std::vector<size_t>& scc) const {
    bool cyclic = false;
    visited[i] = 1;
    for (const auto& x : adjacency_[i]) {
        if (visited[x] == 0 &&
            DirectedGraph::extract_scc_helper(x, visited, pseudo_topo, scc)) {
            cyclic = true;
        }
        if ((visited[x] == 1 || visited[scc[x]] == 1) && scc[i] == i) {
            scc[i] = scc[x];
            cyclic = true;
        }
    }
    visited[i] = 2;
    pseudo_topo.push(i);
    return cyclic;
}
// scc: strongly connected components
std::pair<bool, std::vector<size_t>> DirectedGraph::extract_scc() const {
    const auto N = adjacency_.size();
    std::vector<size_t> scc(N);
    std::vector<size_t> visited(N, 0);
    std::iota(scc.begin(), scc.end(), 0);
    std::stack<size_t> pseudo_topo{};

    bool cyclic = false;
    for (size_t i = 0; i < N; ++i) {
        if (visited[i] == 0 && extract_scc_helper(i, visited, pseudo_topo, scc))
            cyclic = true;
    }

    if (cyclic) {
        while (!pseudo_topo.empty()) {
            auto curr = pseudo_topo.top();
            pseudo_topo.pop();
            scc[curr] = scc[scc[curr]];
        }
    }
    return std::make_pair(cyclic, scc);
}

DirectedGraph DirectedGraph::meta_graph() const {
    bool cyclic;
    std::vector<size_t> scc;
    std::tie(cyclic, scc) = DirectedGraph::extract_scc();
    // auto [cyclic, scc] = DirectedGraph::extract_scc(); // clang bug?
    if (!cyclic) return *this;

    DirectedGraph m_graph{};

    for (auto i = 0; i < adjacency_.size(); ++i) {
        std::for_each(adjacency_[i].begin(), adjacency_[i].end(),
                      [&](const auto& x) {
                          if (scc[i] != scc[x])
                              m_graph.add_edge(std::make_pair(scc[i], scc[x]));
                      });
    }
    return m_graph;
}

void DirectedGraph::extract_sc_helper(
    size_t curr, std::vector<size_t>& chain, std::vector<int>& visited,
    const std::vector<size_t>& scc,
    std::vector<std::vector<size_t>>& cycles) const {
    visited[curr] = 1;
    chain.push_back(curr);

    for (auto x : adjacency_[curr]) {
        if (x == chain[0]) {
            chain.push_back(x);
            cycles.push_back(chain);
            chain.pop_back();
        } else if (scc[x] == scc[chain[0]]) {
            if (visited[x] == 0)
                DirectedGraph::extract_sc_helper(x, chain, visited, scc,
                                                 cycles);
        }
    }

    visited[curr] = 0;
    chain.pop_back();
}

std::vector<std::vector<size_t>> DirectedGraph::extract_simple_cycles() const {
    std::vector<std::vector<size_t>> cycles{};
    auto [cyclic, scc] = DirectedGraph::extract_scc();
    if (!cyclic) return cycles;
    print_elem(scc);

    auto N = adjacency_.size();
    std::vector<size_t> chain{};
    std::vector<int> visited(N, 0);
    for (size_t i = 0; i < adjacency_.size(); ++i) {
        chain.clear();
        std::fill_n(visited.begin(), N, -1);
        std::fill_n(visited.begin() + i, visited.end() - visited.begin() - i,
                    0);
        DirectedGraph::extract_sc_helper(i, chain, visited, scc, cycles);
    }
    return cycles;
}

// for visilization and debug
void DirectedGraph::print_graph() const {
    std::cout << "=========== print adjacency table ==========" << std::endl;

    for (int i = 0; i < adjacency_.size(); ++i) {
        std::cout << i << ": ";
        print_elem<std::set<size_t>>(adjacency_[i]);
    }
}

void DirectedGraph::print_matrix() const {
    // TODO: if calculated already?
    auto matrix = DirectedGraph::extract_matrix();
    // TODO: better visualization
    std::cout << "=========== print graph matrix ==========" << std::endl;
    std::cout << matrix << std::endl;
}

/*

// recursively remove non source-sink nodes in the graph.
// if there are source-sinks left, then the graph is cyclic,
// else the graph is acyclic.
bool DirectedGraph::has_cycle2() const;

DirectedGraph::min_spanning_tree() const;
 */
}  // namespace graph_sdk
