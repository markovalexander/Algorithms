//
//  main.cpp
//  MinBridge
//
//  Created by Александр Марков on 28/11/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>

using std::vector;
vector<bool> used;
vector<int> time_in, f_up;
int timer = 0;

struct edge {
    size_t to;
    int cost;
    
    bool operator<(const edge& other) const {
        return cost < other.cost;
    }
};

void dfs(vector<vector<edge>>* graph, vector<edge>* bridges, size_t vertex, size_t parent = -1) {
    used[vertex] = true;
    time_in[vertex] = f_up[vertex] = timer++;
    for (size_t edge_index = 0; edge_index < (*graph)[vertex].size(); ++edge_index) {
        edge cur_edge = (*graph)[vertex][edge_index];
        if (cur_edge.to == parent) {
            continue;
        }
        if (used[cur_edge.to]) {
            f_up[vertex] = std::min(f_up[vertex], time_in[cur_edge.to]);
        } else {
            dfs(graph, bridges, cur_edge.to, vertex);
            f_up[vertex] = std::min(f_up[vertex], f_up[cur_edge.to]);
            if (f_up[cur_edge.to] > time_in[vertex]) {
                bridges->push_back(cur_edge);
            }
        }
    }
}

vector<edge> find_bridges(vector<vector<edge>>* graph) {
    vector<edge> bridges;
    for (size_t i = 0; i < graph->size(); ++i) {
        if (!used[i]) {
            dfs(graph, &bridges, i);
        }
    }
    return bridges;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    vector<vector<edge>> graph;
    
    size_t n_vertecies, n_edges;
    std::cin >> n_vertecies >> n_edges;
    graph.resize(n_vertecies);
    used.resize(n_vertecies, false);
    time_in.resize(n_vertecies, 0);
    f_up.resize(n_vertecies, 0);
    
    for (size_t i = 0; i < n_edges; ++i) {
        size_t from, to;
        int cost;
        std::cin >> from >> to >> cost;
        graph[from - 1].push_back({to - 1, cost});
        graph[to - 1].push_back({from - 1, cost});
    }
    
    vector<edge> bridges = find_bridges(&graph);
    if (bridges.size() > 0) {
        auto min_most = std::min_element(bridges.begin(), bridges.end());
        std::cout << min_most->cost;
    } else {
        std::cout << -1;
    }
    return 0;
}
