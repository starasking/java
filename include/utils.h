// Copyright All rights reserved.

#ifndef GRAPH_SDK_UTILS_H
#define GRAPH_SDK_UTILS_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <set>


template <typename Container>
void print_elem(const Container& container) {
    std::for_each(container.begin(), container.end(),
                  [](const auto& x) { std::cout << x << ' '; });
    std::cout << std::endl;
}

/*
template <typename K, typename V, typename Map>
bool add_to_map(K key, V value, Map& map_t) {
    if (map_t.find(key) != map_t.end()) {
        auto [insert, state] = map_t[key].insert(value);
        return state;
    } else {
        map_t[key] = std::set<int>{value};
        return true;
    }
}

std::vector<int> extract_path(int node, const std::map<int, int>& upstreams);
*/


#endif
