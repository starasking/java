#include <cmath>
#include <matplot/matplot.h>

std::vector<std::pair<size_t, size_t>> get_edges();

int main() {
    using namespace matplot;
    std::vector<std::pair<size_t, size_t>> edges = {
        {0, 1},   {0, 2},   {0, 3},   {0, 4},   {1, 5},  {1, 6},  {1, 7},
        {1, 8},   {1, 9},   {1, 10},  {1, 11},  {1, 12}, {1, 13}, {1, 14},
        {14, 15}, {14, 16}, {14, 17}, {14, 18}, {14, 19}};
    digraph(edges);

    show();
    return 0;
}

/*
#include <unistd.h>

#include <iostream>

#include "../include/graph.h"
#include "../include/paint.h"
#include "../include/utils.h"

int main() {
    std::cout << "Hello Graph!\n";
    // graph_sdk::Edges edges = {{0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}};
    // graph_sdk::DirectedGraph g1{edges};
    // graph_sdk::DirectedGraph g1{};
    // g1.random_generate_dag(10, 3);
    // graph_sdk::Edges edges = {{0, 9}, {1, 7}, {0, 2}, {2, 3}, {2, 9}, {3, 9},
    //{4, 0}, {5, 3}, {1, 6}, {6, 3}, {6, 4},
    //{7, 4}, {7, 5}, {8, 1}, {8, 0}, {8, 6}};
    graph_sdk::Edges edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 1}};
    graph_sdk::DirectedGraph g1{edges};
    auto [has_cycle, scc] = g1.extract_scc();
    if(has_cycle)
    {
        graph_sdk::print_elem(scc);
    }
    // g1.reset();
    //g1.print_graph();

    //g1 = g1.generate_bipartite_dag();
    //g1.print_graph();

    //auto m1 = g1.extract_di_matrix();
    // m1.print();
    //auto r1 = paint_graph(m1);
    //auto [idx1, nb1] = r1.row_sort_with_indice();


    bool equal = true;
    int i = 0;
    const size_t N = 100;
    while (equal) {
        srand(time(NULL));  // use current time as seed for random generator
        size_t V = rand() % (N -1) + 1;
        g1.random_generate_dag(10, 2);
        std::cout << "edge num = " << g1.fetch_edge_num() << std::endl;

        g1.reset();
        g1.print_graph();
        g1 = g1.generate_bipartite_dag();
        g1.print_graph();

        auto m1 = g1.extract_di_matrix();
        m1.print();

        // auto edge_color = paint_graph(dmatrix);
        auto r1 = paint_graph(m1);
        auto [idx1, nb1] = r1.row_sort_with_indice();
        nb1.print();
        printf("==================\n");

        auto g2 = g1.graph_shuffle();
        g2.print_graph();
        auto m2 = g2.extract_di_matrix();
        auto r2 = paint_graph(m2);
        auto [idx2, nb2] = r2.row_sort_with_indice();
        nb2.print();
        printf("==================\n");
        ++i;
        std::cout << i << ": " << (nb1 == nb2) << std::endl;
        equal = (nb1 == nb2);
        std::getchar();
        //usleep(3000000);
    }
}
*/
