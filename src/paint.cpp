// Copyright (c) 2024 Xuemei Wang. All rights reserved.

#include "../include/paint.h"

#include "../include/graph.h"

namespace graph_sdk {

std::vector<size_t> extract_sources(const Matrix<int>& di_matrix) {
    std::vector<size_t> sources{};
    for (size_t r = 0; r < di_matrix.rows(); ++r) {
        if (di_matrix.row(r).is_none([](int x) { return x < 0; }) &&
            !(di_matrix.row(r).is_all(0))) {
            sources.push_back(r);
        }
    }
    return sources;
}

std::vector<size_t> extract_sinks(const Matrix<int>& di_matrix) {
    std::vector<size_t> sinks{};
    for (size_t r = 0; r < di_matrix.rows(); ++r) {
        if (di_matrix.row(r).is_none([](int x) { return x > 0; }) &&
            !(di_matrix.row(r).is_all(0))) {
            sinks.push_back(r);
        }
    }
    return sinks;
}

Matrix<int> remove_sources(const std::vector<size_t>& sources,
                           const Matrix<int>& di_matrix) {
    auto residual_matrix = di_matrix;
    for (auto s : sources) {
        residual_matrix.col_replace_if(s, 0, [](int x) { return x != 0; });
        residual_matrix.row_replace_if(s, 0, [](int x) { return x != 0; });
    }
    return residual_matrix;
}

std::vector<int> extract_node_description(
    const size_t node, const Matrix<int>& edge_color_matrix) {
    auto description = edge_color_matrix.row(node).collect_element_if(
        [](int x) { return x != 0; });
    return std::vector<int>(description.begin(), description.end());
}

std::tuple<std::set<std::pair<size_t, size_t>>,
           std::unordered_map<size_t, std::vector<int>>>
extract_link_info(const std::vector<size_t>& sources,
                  const Matrix<int>& edge_color_matrix) {
    std::set<std::pair<size_t, size_t>> edges;
    std::unordered_map<size_t, std::vector<int>> node_dictionary;
    for (auto down_s : sources) {
        auto nd1 = extract_node_description(down_s, edge_color_matrix);
        node_dictionary.insert({down_s, nd1});
        auto up_streams = edge_color_matrix.col(down_s).find_all(
            [](const auto& x) { return x < 0; });
        for (auto up_s : up_streams) {
            auto nd2 = extract_node_description(up_s, edge_color_matrix);
            edges.insert({up_s, down_s});
            node_dictionary.insert({up_s, nd2});
        }
    }
    return std::make_tuple(edges, node_dictionary);
}

std::set<std::pair<size_t, size_t>, SecondCom> sort_by_number(
    const std::multiset<size_t>& nodes) {
    std::unordered_map<size_t, size_t> statistic_nodes{};
    for (auto node : nodes) {
        if (auto it = statistic_nodes.find(node); it == statistic_nodes.end()) {
            statistic_nodes.insert({node, 1});
        } else {
            it->second += 1;
        }
    }
    std::set<std::pair<size_t, size_t>, SecondCom> node_with_num{};
    for (auto it = statistic_nodes.begin(); it != statistic_nodes.end(); ++it) {
        node_with_num.insert({it->first, it->second});
    }
    return node_with_num;
}

std::multiset<
    std::pair<std::pair<size_t, size_t>, std::vector<std::vector<int>>>,
    Vec2Com>
extract_edge_description(
    const std::set<std::pair<size_t, size_t>>& edges,
    std::unordered_map<size_t, std::vector<int>>& node_dictionary) {
    std::multiset<
        std::pair<std::pair<size_t, size_t>, std::vector<std::vector<int>>>,
        Vec2Com>
        edge_description{};

    for (auto edge : edges) {
        std::vector<std::vector<int>> edge_descriptor{};
        edge_descriptor.push_back(node_dictionary[edge.first]);
        edge_descriptor.push_back(node_dictionary[edge.second]);
        edge_description.insert({edge, edge_descriptor});
    }
    return edge_description;
}

void update_edge_color(const std::vector<size_t>& sources,
                       Matrix<int>& edge_color_matrix, int& max_edge_color,
                       std::unordered_set<int>& label_set) {
    auto [edges, node_dictionary] =
        extract_link_info(sources, edge_color_matrix);
    auto edge_description_multiset =
        extract_edge_description(edges, node_dictionary);
    auto it_start = edge_description_multiset.begin();

    std::vector<std::vector<int>> previous_descriptor{};
    std::unordered_set<int> new_label_set{};

    for (auto it = it_start; it != edge_description_multiset.end(); ++it) {
        auto it_prev = (it == it_start) ? it : std::prev(it, 1);
        if (it->second == previous_descriptor) {
            edge_color_matrix(it->first.first, it->first.second) =
                edge_color_matrix(it_prev->first.first, it_prev->first.second);
            edge_color_matrix(it->first.second, it->first.first) =
                edge_color_matrix(it_prev->first.second, it_prev->first.first);
        } else {
            if (label_set.find(edge_color_matrix(
                    it->first.first, it->first.second)) != label_set.end()) {
                max_edge_color += 1;
                edge_color_matrix(it->first.first, it->first.second) =
                    -max_edge_color;
                edge_color_matrix(it->first.second, it->first.first) =
                    max_edge_color;
            } else if (it != it_start &&
                       edge_color_matrix(it->first.first, it->first.second) ==
                           edge_color_matrix(it_prev->first.first,
                                             it_prev->first.second)) {
                max_edge_color += 1;
                edge_color_matrix(it->first.first, it->first.second) =
                    -max_edge_color;
                edge_color_matrix(it->first.second, it->first.first) =
                    max_edge_color;
            } else if (new_label_set.find(edge_color_matrix(
                           it->first.first, it->first.second)) !=
                       new_label_set.end()) {
                max_edge_color += 1;
                edge_color_matrix(it->first.first, it->first.second) =
                    -max_edge_color;
                edge_color_matrix(it->first.second, it->first.first) =
                    max_edge_color;
            }
        }
        new_label_set.insert(
            edge_color_matrix(it->first.first, it->first.second));
        previous_descriptor = it->second;
    }
    std::copy(new_label_set.begin(), new_label_set.end(),
              std::inserter(label_set, label_set.end()));
}

void paint_once(Matrix<int>& edge_color_matrix, int& max_edge_color,
                Matrix<int>& curr_matrix) {
    auto sinks = extract_sinks(curr_matrix);
    std::unordered_set<int> label_set{};
    while (!curr_matrix.is_all(0)) {
        auto sources = extract_sources(curr_matrix);
        update_edge_color(sources, edge_color_matrix, max_edge_color,
                          label_set);
        curr_matrix = remove_sources(sources, curr_matrix);
    }
    update_edge_color(sinks, edge_color_matrix, max_edge_color, label_set);
}

Matrix<int> generate_description(const Matrix<int>& edge_color_matrix) {
    auto row_num = edge_color_matrix.rows();
    std::vector<std::vector<int>> result(row_num, std::vector<int>{});
    size_t max_cols = 0;
    for (auto r = 0; r < row_num; ++r) {
        result[r] = extract_node_description(r, edge_color_matrix);
        max_cols = (max_cols < result[r].size()) ? result[r].size() : max_cols;
    }
    for (auto r = 0; r < row_num; ++r) {
        result[r].resize(max_cols);
    }
    Matrix<int> matrix(result);
    return matrix;
}

Matrix<int> paint_graph(const Matrix<int>& di_matrix) {
    int max_edge_color = 1;
    auto edge_color_matrix = di_matrix;
    auto curr_matrix = di_matrix;
    bool loop = true;
    while (loop) {
        auto tmp = edge_color_matrix;
        paint_once(edge_color_matrix, max_edge_color, curr_matrix);
        generate_description(edge_color_matrix).print();
        loop = (tmp != edge_color_matrix);
        edge_color_matrix = edge_color_matrix.transpose();
        curr_matrix = di_matrix.transpose();
    }
    return generate_description(edge_color_matrix);
}
}  // namespace graph_sdk
