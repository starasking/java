// Copyright All rights reserved.

#ifndef GRAPH_SDK_UTILS_H
#define GRAPH_SDK_UTILS_H

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace graph_sdk {
template <typename Container>
void print_elem(const Container& container) {
    std::for_each(container.begin(), container.end(),
                  [](const auto& x) { std::cout << x << ' '; });
    std::cout << std::endl;
}

template <typename Container2d>
void print_elem_2d(const Container2d& container2d) {
    std::for_each(container2d.begin(), container2d.end(),
                  [](const auto& x) {print_elem(x); });
}
}  // namespace graph_sdk

#endif
