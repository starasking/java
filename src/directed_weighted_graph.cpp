#include "../include/graph.h"

namespace graph_sdk {
DiWeightedGraph::DiWeightedGraph(const WeightedEdges& edges) {
    assert(adjacency_.empty());
    assert(weighted_edges_.empty());

    weighted_edges_ = edges;
    for (const auto& [key, value] : weighted_edges_) {
        auto size = std::max(key.first, key.second);
        if (size > adjacency_.size()) {
            adjacency_.resize(size);
        }
        adjacency_[key.first].insert(key.second);
    }
}

bool DiWeightedGraph::add_edge(std::tuple<size_t, size_t, int> weighted_edge) {
    auto [p0, p1, v] = weighted_edge;
    if (auto max_tmp = std::max(p0, p1); max_tmp >= adjacency_.size()) {
        adjacency_.resize(max_tmp + 1);
    }
    auto tmp1 = adjacency_[p0].insert(p1);
    if (tmp1.second) {
        auto tmp2 = weighted_edges_.insert({std::make_pair(p0, p1), v});
        return tmp2.second;
    } else
        return false;
}
bool DiWeightedGraph::remove_node(size_t idx) {
    auto N = adjacency_.size();
    if (idx >= N) return false;

    for (auto i = 0; i < N; ++i) {
        if (idx == i) {
            for (auto x : adjacency_[i]) {
                weighted_edges_.erase(
                    weighted_edges_.find(std::make_pair(i, x)));
            }
            if (idx == N - 1) {
                adjacency_.resize(N - 1);
            } else {
                adjacency_[i] = {};
            }
        } else {
            if (auto it = adjacency_[i].find(idx); it != adjacency_[i].end()) {
                weighted_edges_.erase(
                    weighted_edges_.find(std::make_pair(i, *it)));
                adjacency_[i].erase(it);
            }
        }
    }
    return true;
}
 bool DiWeightedGraph::remove_edge(std::pair<size_t, size_t> arrow)
{
    if (arrow.first >= adjacency_.size()) return false;
    if (auto it = adjacency_[arrow.first].find(arrow.second);
        it != adjacency_[arrow.first].end()) {
        adjacency_[arrow.first].erase(it);
        weighted_edges_.erase(weighted_edges_.find(arrow));
        return true;
    } else
        return false;
}
}  // namespace graph_sdk
