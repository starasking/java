#include <iostream>

#include "../include/graph.h"
#include "../include/utils.h"

int main() {
    std::cout << "Hello Graph!\n";

    // graph_sdk::Edges edges = {{0, 1}, {1, 2}, {2, 0}};//3}, {3, 4}, {4, 0}};
    // graph_sdk::Edges edges = {{0, 1}, {1, 2}, {1, 4}, {4, 2}, {4, 3}, {3, 5},
    //{5, 2}, {3, 2}, {8, 6}, {6, 3}};//, {5, 8}};
    // graph_sdk::DirectedGraph dgraph(edges);

    graph_sdk::DirectedGraph dgraph{};
    //dgraph.random_generate(10, 2);
    dgraph.random_generate_dag(20, 7);
    dgraph.print_graph();
    dgraph.extract_matrix().raw_print();

    /*
    if (dgraph.has_cycle()) {
        auto circles = dgraph.extract_simple_cycles();
        printf("There are %lu cycles in the graph.\n", circles.size());
        for (auto x: circles)
            print_elem(x);
    } else {
        printf("There are No cycles in the graph.\n");
    }
    */



    // std::cout <<  << std::endl;
    //  auto dfs_nodes = dgraph2.dfs();
    //  print_elem(dfs_nodes);
    // auto [is_acyclic, topo_sorted] = dgraph.topological_sort();
    // std::cout << is_acyclic << std::endl;
    // if (is_acyclic) {
    // while (!topo_sorted.empty()) {
    // auto tmp = topo_sorted.top();
    // topo_sorted.pop();
    // std::cout << tmp << ' ';
    //}
    // std::cout << std::endl;
    //}

    // auto [cyclic, scc] = dgraph.extract_scc();
    // print_elem(scc);
    // auto m_graph = dgraph.meta_graph();
    // m_graph.print_graph();

    /*
    graph_sdk::DirectedGraph dgraph{};
    //state = dgraph.remove_edge(5, 2);
    dgraph.print_graph();
    dgraph.print_nodes();
    //std::cout << topo_result.first << std::endl;
    //std::cout << dgraph.has_cycle() << std::endl;
    //graph_sdk::DirectedGraph dg2{std::move(dgraph)};
    */
}
