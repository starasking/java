// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#ifndef GRAPH_SDK_PAINT_H
#define GRAPH_SDK_PAINT_H

#include <algorithm>
#include <functional>
#include <numeric>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "../include/graph.h"

namespace graph_sdk {

struct SecondCom {
    template <typename t1, typename t2>
    bool operator()(const std::pair<t1, t2>& lhs,
                    const std::pair<t1, t2>& rhs) const {
        return lhs.second <= rhs.second;
    }
};

struct Vec2Com {
    template <typename T, typename C>
    bool operator()(const std::pair<T, C>& lhs,
                    const std::pair<T, C>& rhs) const {
        auto [lt, lc] = lhs;
        auto [rt, rc] = rhs;

        assert(lc.size() == rc.size());
        for (auto i = 0; i < lc.size(); ++i) {
            if (lc[i].size() != rc[i].size())
                return (lc[i].size() < rc[i].size());
            if (!std::equal(lc[i].begin(), lc[i].end(), rc[i].begin()))
                return std::lexicographical_compare(lc[i].begin(), lc[i].end(),
                                                    rc[i].begin(), rc[i].end());
        }
        return false;
    }
};

Matrix<int> remove_sources(const std::vector<size_t>& sources,
                           const Matrix<int>& di_matrix);

std::vector<int> extract_node_description(const size_t node,
                                          const Matrix<int>& edge_color_matrix);

std::vector<size_t> extract_sources(const Matrix<int>& di_matrix);

std::tuple<std::set<std::pair<size_t, size_t>>,
           std::unordered_map<size_t, std::vector<int>>>
extract_link_info(const std::vector<size_t>& sources,
                  const Matrix<int>& edge_color_matrix);

std::multiset<
    std::pair<std::pair<size_t, size_t>, std::vector<std::vector<int>>>,
    Vec2Com>
extract_edge_description(
    const std::set<std::pair<size_t, size_t>>& edges,
    std::unordered_map<size_t, std::vector<int>>& node_dictionary);

void update_edge_color(const std::vector<size_t>& sources,
                       Matrix<int>& edge_color_matrix, int& max_edge_color,
                       std::unordered_set<int>& label_set);

void paint_once(Matrix<int>& edge_color_matrix, int& max_edge_color,
                Matrix<int>& curr_matrix);

Matrix<int> paint_graph(const Matrix<int>& di_matrix);

Matrix<int> generate_description(const Matrix<int>& edge_color_matrix);

}  // namespace graph_sdk
#endif
