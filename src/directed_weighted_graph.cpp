#include <limits>

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

    VN_ = adjacency_.size();
    EN_ = weighted_edges_.size();
}

bool DiWeightedGraph::add_edge(std::tuple<size_t, size_t, int> weighted_edge) {
    auto [p0, p1, v] = weighted_edge;
    if (auto max_tmp = std::max(p0, p1); max_tmp >= adjacency_.size()) {
        adjacency_.resize(max_tmp + 1);
        VN_ += 1;
    }
    auto tmp1 = adjacency_[p0].insert(p1);
    if (tmp1.second) {
        auto tmp2 = weighted_edges_.insert({std::make_pair(p0, p1), v});
        if (tmp2.second) EN_ += 1;
        return tmp2.second;
    } else
        return false;
}
bool DiWeightedGraph::remove_node(size_t idx) {
    auto N = VN_;
    if (idx >= N) return false;

    for (auto i = 0; i < N; ++i) {
        if (idx == i) {
            for (auto x : adjacency_[i]) {
                weighted_edges_.erase(
                    weighted_edges_.find(std::make_pair(i, x)));
            }
            if (idx == N - 1) {
                adjacency_.resize(N - 1);
                VN_ -= 1;
            } else {
                adjacency_[i] = {};
            }
            EN_ -= adjacency_[i].size();
        } else {
            if (auto it = adjacency_[i].find(idx); it != adjacency_[i].end()) {
                weighted_edges_.erase(
                    weighted_edges_.find(std::make_pair(i, *it)));
                adjacency_[i].erase(it);
                EN_ -= 1;
            }
        }
    }
    return true;
}
bool DiWeightedGraph::remove_edge(std::pair<size_t, size_t> arrow) {
    if (arrow.first >= adjacency_.size()) return false;
    if (auto it = adjacency_[arrow.first].find(arrow.second);
        it != adjacency_[arrow.first].end()) {
        adjacency_[arrow.first].erase(it);
        weighted_edges_.erase(weighted_edges_.find(arrow));
        EN_ -= 1;
        return true;
    } else
        return false;
}

bool DiWeightedGraph::is_positive_weighted() const {
    return std::all_of(adjacency_.begin(), adjacency_.end(), [](auto const& x) {
        return std::all_of(x.begin(), x.end(),
                           [](auto const& elem) { return elem > 0; });
    });
}

bool DiWeightedGraph::update_flow_graph(
    size_t source, size_t sink,
    std::vector<std::unordered_map<size_t, int>>& flow_adjacency,
    int& flow_value) const {
    std::vector<size_t> path{};
    find_one_path_helper(source, sink, flow_adjacency, path);
    if (path.empty()) return false;

    int min_value = std::numeric_limits<int>::max();
    for (size_t i = 0; i < path.size() - 1; ++i) {
        auto value = flow_adjacency[path[i]][path[i + 1]];
        min_value = min_value < value ? min_value : value;
    }
    flow_value += min_value;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        assert(flow_adjacency[path[i]][path[i + 1]] >= min_value);
        if (flow_adjacency[path[i]][path[i + 1]] == min_value) {
            flow_adjacency[path[i]].erase(
                flow_adjacency[path[i]].find(path[i + 1]));
        } else {
            flow_adjacency[path[i]][path[i + 1]] -= min_value;
        }
        if (auto it = flow_adjacency[path[i + 1]].find(path[i]);
            it == flow_adjacency[path[i + 1]].end()) {
            flow_adjacency[path[i + 1]].insert({path[i], min_value});
        } else {
            flow_adjacency[path[i + 1]][path[i]] += min_value;
        }
    }
    return true;
}

void DiWeightedGraph::find_one_path_helper(
    size_t source, size_t sink,
    const std::vector<std::unordered_map<size_t, int>>& flow_adjacency,
    std::vector<size_t>& path) const {
    path.push_back(source);
    if (source == sink) return;
    for (const auto& [id, value] : flow_adjacency[source]) {
        find_one_path_helper(id, sink, flow_adjacency, path);
    }
    path.pop_back();
}

std::vector<std::unordered_map<size_t, int>>
DiWeightedGraph::get_weighted_adjacency() const {
    std::vector<std::unordered_map<size_t, int>> weighted_adjacency(VN_);
    for (size_t i = 0; i < VN_; ++i) {
        std::for_each(adjacency_[i].begin(), adjacency_[i].end(),
                      [&](const auto& x) {
                          auto it = weighted_edges_.find(std::make_pair(i, x));
                          assert(it != weighted_edges_.end());
                          weighted_adjacency[i].insert({x, it->second});
                      });
    }
    return weighted_adjacency;
}

int DiWeightedGraph::max_flow(size_t source, size_t sink) const {
    int flow_value = 0;
    std::vector<std::unordered_map<size_t, int>> flow_adjacency =
        DiWeightedGraph::get_weighted_adjacency();

    bool update = true;
    while (update) {
        update = DiWeightedGraph::update_flow_graph(source, sink,
                                                    flow_adjacency, flow_value);
    }
    return flow_value;
}

}  // namespace graph_sdk
