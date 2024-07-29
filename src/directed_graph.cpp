// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <stack>
#include <unordered_map>
#include <utility>

#include "../include/graph.h"
#include "../include/utils.h"

namespace graph_sdk {

// generation
DirectedGraph::DirectedGraph(const size_t N) {
    adjacency_ = std::vector<std::set<size_t>>(N, std::set<size_t>{});
    VN_ = N;
}

DirectedGraph::DirectedGraph(const Edges& edges) {
    auto node = *edges.rbegin();
    adjacency_.resize(std::max(node.first, node.second) + 1);

    std::for_each(edges.begin(), edges.end(),
                  [&](const auto& x) { adjacency_[x.first].insert(x.second); });
    VN_ = adjacency_.size();
    EN_ = edges.size();
}

DirectedGraph::DirectedGraph(const Matrix<size_t>& matrix) {
    // TODO: elem: 0 or 1;
    assert(matrix.rows() == matrix.cols());
    VN_ = matrix.cols();
    EN_ = 0;
    adjacency_.resize(VN_);
    for (auto i = 0; i < VN_; ++i) {
        for (auto j = 0; j < VN_; ++j) {
            if (matrix(i, j) == 1) {
                adjacency_[i].insert(j);
                EN_ += 1;
            }
        }
    }
}

void DirectedGraph::random_generate(size_t V, size_t D) {
    srand(time(NULL));
    adjacency_.assign(V, std::set<size_t>{});
    VN_ = V;
    EN_ = 0;

    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) continue;
            if (rand() % V < D) {
                adjacency_[i].insert(j);
                EN_ += 1;
            }
        }
    }
}
// modification
bool DirectedGraph::add_edge(std::pair<size_t, size_t> arrow) {
    if (auto max_tmp = std::max(arrow.first, arrow.second); max_tmp >= VN_) {
        adjacency_.resize(max_tmp + 1);
    }
    auto tmp = adjacency_[arrow.first].insert(arrow.second);
    VN_ = adjacency_.size();
    if (tmp.second) EN_ += 1;
    return tmp.second;
}

bool DirectedGraph::remove_node(size_t idx) {
    if (idx >= VN_) return false;

    if (idx == VN_ - 1) {
        adjacency_.resize(idx);
        VN_ -= 1;
        EN_ -= adjacency_[idx].size();
    } else if (!adjacency_[idx].empty()) {
        adjacency_[idx] = {};
        EN_ -= adjacency_[idx].size();
    }

    std::for_each(adjacency_.begin(), adjacency_.end(), [&](auto& x) {
        if (auto it = x.find(idx); it != x.end()) {
            x.erase(it);
            EN_ -= 1;
        }
    });
    return true;
}

bool DirectedGraph::remove_edge(std::pair<size_t, size_t> arrow) {
    if (arrow.first >= VN_) return false;
    if (auto it = adjacency_[arrow.first].find(arrow.second);
        it != adjacency_[arrow.first].end()) {
        adjacency_[arrow.first].erase(it);
        EN_ -= 1;
        return true;
    } else
        return false;
}

bool DirectedGraph::random_remove_edges(size_t n) {
    if (n > EN_) return false;
    Edges edges = DirectedGraph::extract_edges();

    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};

    std::vector<size_t> tmp(EN_, 0);
    std::iota(tmp.begin(), tmp.end(), 0);
    std::shuffle(tmp.begin(), tmp.end(), rng);
    for (auto i = 0; i < n; ++i) {
        auto it = std::next(edges.begin(), tmp[i]);
        remove_edge(*it);
    }
    EN_ -= n;
    return true;
}

void DirectedGraph::exchange_nodes(size_t n1, size_t n2) {
    assert(std::max(n1, n2) < VN_);
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
    const auto N = VN_;

    for (size_t i = 0; i < N; ++i) {
        if (attributes[i] == NodeAttribute::isolated) {
            missed.insert(i);
        }
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
    VN_ = adjacency_.size() - missed.size();
    adjacency_.resize(VN_);

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
    EN_ = DirectedGraph::calculate_edge_num();
}

// representation
Matrix<size_t> DirectedGraph::extract_matrix() const {
    auto matrix = Matrix<size_t>(VN_, VN_);
    for (size_t i = 0; i < VN_; ++i) {
        for (const auto& x : adjacency_[i]) {
            matrix(i, x) = 1;
        }
    }
    return matrix;
}
Matrix<int> DirectedGraph::extract_di_matrix() const {
    auto matrix = Matrix<int>(VN_, VN_);
    for (size_t i = 0; i < VN_; ++i) {
        for (const auto& x : adjacency_[i]) {
            matrix(i, x) = 1;
            matrix(x, i) = -1;
        }
    }
    return matrix;
}

size_t DirectedGraph::fetch_edge_num() const { return EN_; }

size_t DirectedGraph::calculate_edge_num() const {
    size_t en = 0;
    std::for_each(adjacency_.begin(), adjacency_.end(),
                  [&](const auto& x) { en += x.size(); });
    return en;
}
Edges DirectedGraph::extract_edges() const {
    Edges edges;
    for (size_t i = 0; i < VN_; ++i) {
        for (const auto& y : adjacency_[i]) edges.insert({i, y});
    }
    return edges;
}

std::vector<NodeAttribute> DirectedGraph::get_attribute() const {
    std::vector<NodeAttribute> attribute(VN_, NodeAttribute::unlabelled);
    for (auto i = 0; i < VN_; ++i) {
        if (adjacency_[i].empty()) {
            if (attribute[i] == NodeAttribute::unlabelled) {
                attribute[i] = NodeAttribute::isolated;
            }
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

DirectedGraph DirectedGraph::reverse_graph() const {
    DirectedGraph g{VN_};
    for (auto i = 0; i < VN_; ++i) {
        std::for_each(adjacency_[i].begin(), adjacency_[i].end(),
                      [&](const auto& x) { g.adjacency_[x].insert(i); });
    }

    return g;
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
    std::vector<bool> visited(VN_, false);
    std::vector<size_t> dfs_nodes{};

    for (auto i = 0; i < VN_; ++i) {
        if (!visited[i]) DirectedGraph::dfs_helper(i, visited, dfs_nodes);
    }
    return dfs_nodes;
}

bool DirectedGraph::topo_sort_helper(size_t i,
                                     std::stack<size_t>& reverse_ordered,
                                     std::vector<size_t>& visited) const {
    visited[i] = 1;
    for (const auto x : adjacency_[i]) {
        if (visited[x] == 1 ||
            (visited[x] == 0 &&
             !DirectedGraph::topo_sort_helper(x, reverse_ordered, visited)))
            return false;
    }
    visited[i] = 2;
    reverse_ordered.push(i);
    return true;
}

std::pair<bool, std::stack<size_t>> DirectedGraph::topological_sort() const {
    std::stack<size_t> reverse_ordered{};
    std::vector<size_t> visited(VN_, 0);
    for (auto i = 0; i < VN_; ++i) {
        if (visited[i] == 0 &&
            !DirectedGraph::topo_sort_helper(i, reverse_ordered, visited))
            return std::make_pair(false, std::stack<size_t>{});
    }
    return std::make_pair(true, reverse_ordered);
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

void DirectedGraph::random_generate_dag(size_t V, size_t D) {
    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};

    std::vector<size_t> tmp(V);
    std::iota(tmp.begin(), tmp.end(), 0);
    std::shuffle(tmp.begin(), tmp.end(), rng);
    adjacency_.assign(V, std::set<size_t>{});
    VN_ = V;
    EN_ = 0;

    for (auto i : tmp) {
        for (int j = 0; j < V; j++) {
            if (i == j) continue;
            adjacency_[i].insert(j);
            EN_ += 1;
            if (DirectedGraph::has_cycle() || rand() % V >= D) {
                auto it = adjacency_[i].find(j);
                adjacency_[i].erase(it);
                EN_ -= 1;
            }
        }
    }
}

DirectedGraph DirectedGraph::generate_bipartite_dag() const {
    DirectedGraph graph{VN_ + EN_};
    graph.VN_ = VN_ + EN_;
    graph.EN_ = 2 * EN_;
    size_t n = VN_;

    for (size_t i = 0; i < VN_; ++i) {
        std::for_each(adjacency_[i].begin(), adjacency_[i].end(),
                      [&](const auto& x) {
                          graph.adjacency_[n].insert(i);
                          graph.adjacency_[n].insert(x);
                          n += 1;
                      });
    }
    return graph;
}

DirectedGraph DirectedGraph::graph_shuffle() const {
    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};
    std::vector<size_t> tmp(adjacency_.size());
    std::iota(tmp.begin(), tmp.end(), 0);
    std::shuffle(tmp.begin(), tmp.end(), rng);
    // print_elem(tmp);

    DirectedGraph graph;
    for (auto i = 0; i < adjacency_.size(); ++i) {
        for (auto x : adjacency_[i]) {
            graph.add_edge({tmp[i], tmp[x]});
        }
    }

    return graph;
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
    std::vector<size_t> scc(VN_);
    std::vector<size_t> visited(VN_, 0);
    std::iota(scc.begin(), scc.end(), 0);
    std::stack<size_t> pseudo_topo{};

    bool cyclic = false;
    for (size_t i = 0; i < VN_; ++i) {
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
    size_t curr, std::vector<size_t>& chain, std::vector<bool>& valid,
    const std::vector<size_t>& scc,
    std::vector<std::vector<size_t>>& cycles) const {
    chain.push_back(curr);

    for (auto x : adjacency_[curr]) {
        if (x == chain[0]) {
            chain.push_back(x);
            cycles.push_back(chain);
            chain.pop_back();
        } else if (scc[x] == scc[chain[0]] && valid[x]) {
            DirectedGraph::extract_sc_helper(x, chain, valid, scc, cycles);
        }
    }
    chain.pop_back();
}

std::vector<std::vector<size_t>> DirectedGraph::extract_simple_cycles() const {
    std::vector<std::vector<size_t>> cycles{};
    auto [cyclic, scc] = DirectedGraph::extract_scc();
    if (!cyclic) return cycles;
    // print_elem(scc);

    std::vector<size_t> chain{};
    std::vector<bool> valid(VN_, false);
    for (size_t i = 0; i < adjacency_.size(); ++i) {
        chain.clear();
        std::fill_n(valid.begin(), VN_, false);
        std::fill_n(valid.begin() + i, valid.end() - valid.begin() - i, true);
        DirectedGraph::extract_sc_helper(i, chain, valid, scc, cycles);
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
    // std::for_each(matrix.begin(), matrix.end(),
    //[](const auto& x){print_elem(x);});
    matrix.print();
}

/*

// recursively remove non source-sink nodes in the graph.
// if there are source-sinks left, then the graph is cyclic,
// else the graph is acyclic.
bool DirectedGraph::has_cycle2() const;

DirectedGraph::min_spanning_tree() const;
 */
}  // namespace graph_sdk
