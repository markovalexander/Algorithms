//
//  main.cpp
//  AgentsMST
//
//  Created by Александр Марков on 17/12/2017.
//  Copyright © 2017. All rights reserved.
//

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

class DSU {
public:
    DSU() {}
    
    explicit DSU(size_t dsu_size) {
        parents_.resize(dsu_size);
        std::iota(parents_.begin(), parents_.end(), 0);
    }
    
    void Add() {
        parents_.push_back(parents_.size());
    }
    
    size_t Find(size_t index) {
        if (parents_[index] == index) {
            return index;
        } else {
            return parents_[index] = Find(parents_[index]);
        }
    }
    
    void Unite(size_t first_set_elem, size_t second_set_elem) {
        size_t set_index = generator() % 2;
        size_t first_set_index = Find(first_set_elem);
        size_t second_set_index = Find(second_set_elem);
        
        if (set_index == 0) {
            parents_[second_set_index] = first_set_index;
        } else {
            parents_[first_set_index] = second_set_index;
        }
    }
    
private:
    std::vector<size_t> parents_;
    std::default_random_engine generator;
};

struct Edge {
    size_t start;
    size_t finish;
    int64_t cost;
};

bool EdgeComparator(const Edge& first, const Edge& second) {
    return first.cost < second.cost;
}

struct Point {
    int64_t first_coordinate;
    int64_t second_coordinate;
};

using Graph = std::vector<Edge>;

int64_t distance(Point first, Point second) {
    int64_t first_c = (first.first_coordinate - second.first_coordinate)
                    * (first.first_coordinate - second.first_coordinate);
    
    int64_t second_c = (first.second_coordinate - second.second_coordinate)
                        * (first.second_coordinate - second.second_coordinate);
    return first_c + second_c;
}

Graph createGraph(const std::vector<Point>& points) {
    Graph graph(points.size());
    for (size_t first = 0; first < points.size(); ++first) {
        Point current_agent = points[first];
        for (size_t second = first + 1; second < points.size(); ++second) {
            graph.push_back({first, second, distance(current_agent, points[second])});
        }
    }
    return graph;
}

Graph findMST(size_t n_vertices, Graph* graph) {
    std::sort((*graph).begin(), (*graph).end(), EdgeComparator);
    Graph MST;
    DSU vertices_dsu(n_vertices);
    for (size_t edge_index = 0; edge_index < (*graph).size(); ++edge_index) {
        size_t start = (*graph)[edge_index].start;
        size_t finish = (*graph)[edge_index].finish;
        
        if (vertices_dsu.Find(start) != vertices_dsu.Find(finish)) {
            MST.push_back((*graph)[edge_index]);
            vertices_dsu.Unite(start, finish);
        }
    }
    return MST;
}

std::vector<Point> ReadPoints() {
    size_t n_points;
    std::cin >> n_points;
    
    std::vector<Point> points;
    for (size_t i = 0; i < n_points; ++i) {
        int64_t first, second;
        std::cin >> first >> second;
        points.push_back({first, second});
    }
    return points;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie();
    
    std::vector<Point> agents = ReadPoints();
    Graph agents_graph = createGraph(agents);
    Graph min_spanning_tree = findMST(agents.size(), &agents_graph);
    int64_t max_edge = std::max_element(min_spanning_tree.begin(), min_spanning_tree.end(),
                                        EdgeComparator)->cost;
    double answer = sqrt(static_cast<double>(max_edge));
    printf("%.10f", answer);
    return 0;
}
