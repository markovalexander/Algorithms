    //
    //  main.cpp
    //  MaximalCompany
    //
    //  Created by Александр Марков on 28/11/2017.
    //  Copyright © 2017. All rights reserved.
    //

#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>

namespace iterators {
    
template<typename Iterator>
class RangeIterator {
public:
    RangeIterator(Iterator begin, Iterator end)
    : begin_(begin), end_(end) {}
    
    Iterator begin() const {
        return begin_;
    }
    
    Iterator end() const {
        return end_;
    }
private:
    Iterator begin_, end_;
};

template<typename EnumType>
class EnumerationIterator {
public:
    explicit EnumerationIterator(EnumType begin)
    : current_state(begin) {}
    
    EnumType& operator*() {
        return current_state;
    }
    
    EnumerationIterator operator++() {
        ++current_state;
        return *this;
    }
    
    bool operator==(const EnumerationIterator<EnumType>& other) {
        return current_state == other.current_state;
    }
    
    bool operator!=(const EnumerationIterator<EnumType>& other) {
        return !(*this == other);
    }
    
private:
    EnumType current_state;
};

template<typename Iterator>
RangeIterator<Iterator> MakeRangeIterator(Iterator begin, Iterator end) {
    return RangeIterator<Iterator>(begin, end);
}
    
} // namespace iterators

namespace graph {

using iterators::RangeIterator;
using iterators::EnumerationIterator;
using iterators::MakeRangeIterator;

class Edge {
public:
    using Vertex = size_t;
    
    Edge() {}
    Edge(Vertex start, Vertex finish)
    : start_(start), finish_(finish) {}
    
    Vertex GetStart() const {
        return start_;
    }
    
    Vertex GetFinish() const {
        return finish_;
    }
    
    bool operator==(const Edge& other) const {
        return start_ == other.start_ && finish_ == other.finish_;
    }
    
    bool operator!=(const Edge& other) const {
        return !(*this == other);
    }
    
private:
    Vertex start_, finish_;
};

class EdgeHasher {
public:
    size_t operator()(const Edge& edge) const {
        std::hash<size_t> hasher;
        size_t seed = hasher(edge.GetStart());
        return seed ^= hasher(edge.GetFinish()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};
    
Edge ReverseEdge(const Edge& edge) {
    return Edge(edge.GetFinish(), edge.GetStart());
}

class Graph {
    using Vertex = typename Edge::Vertex;
    using VerticesRange = RangeIterator<EnumerationIterator<Vertex>>;
    using EdgesRange = RangeIterator<typename std::vector<Edge>::const_iterator>;

public:
    Graph() {}
    explicit Graph(size_t num_vertices)
    : outgoing_edges_list_(num_vertices) {}
    
    template<typename EdgeRangeIterator>
    Graph(size_t num_vertices, EdgeRangeIterator EdgesRange)
    : outgoing_edges_list_(num_vertices) {
        for (const Edge& edge: EdgesRange) {
            AddEdge(edge);
        }
    }
    
    void AddEdge(const Edge& edge) {
        outgoing_edges_list_[edge.GetStart()].push_back(edge);
    }
    
    size_t GetVerticesNumber() const {
        return outgoing_edges_list_.size();
    }
    
    VerticesRange GetVertices() const {
        EnumerationIterator<Vertex> begin_posistion(0);
        EnumerationIterator<Vertex> end_position(GetVerticesNumber());
        return MakeRangeIterator(begin_posistion, end_position);
    }
    
    EdgesRange GetEdges(Vertex vertex) const {
        return MakeRangeIterator(outgoing_edges_list_[vertex].begin(),
                                 outgoing_edges_list_[vertex].end());
    }
    
    size_t GetVertexDegree(Vertex vertex) const {
        return outgoing_edges_list_[vertex].size();
    }
    
private:
    std::vector<std::vector<Edge>> outgoing_edges_list_;
};

Graph CreateTransposeGraph(const Graph& graph_to_transpose) {
    Graph transposed_graph(graph_to_transpose.GetVerticesNumber());
    for (const auto& vertex : graph_to_transpose.GetVertices()) {
        for (const auto& edge : graph_to_transpose.GetEdges(vertex)) {
            transposed_graph.AddEdge(ReverseEdge(edge));
        }
    }
    return transposed_graph;
}

Graph CreateCondenceGraph(const Graph& graph_to_condecate,
                          const std::vector<size_t> components_id)
{
    std::unordered_set<Edge, EdgeHasher> condence_graph_edges;
    for (const auto& vertex : graph_to_condecate.GetVertices()) {
        for (const auto& edge : graph_to_condecate.GetEdges(vertex)) {
            if (components_id[edge.GetStart()] != components_id[edge.GetFinish()]) {
                condence_graph_edges.emplace(components_id[edge.GetStart()],
                                             components_id[edge.GetFinish()]);
            }
        }
    }
    size_t number_of_stronly_connected_components = *std::max_element(components_id.begin(),
                                                                      components_id.end());
    return Graph(number_of_stronly_connected_components + 1,
                 MakeRangeIterator(condence_graph_edges.begin(), condence_graph_edges.end()));
}

} // namespace graph

namespace visitors {

template<typename VertexType, typename EdgeType>
class DfsVisitor {
public:
    virtual void DiscoverComponent() {}
    virtual void DiscoverVertex(VertexType vertex) {}
    virtual void ExamineEdge(const EdgeType& edge) {}
    virtual void ExitVertex(VertexType vertex) {}
    virtual ~DfsVisitor() {}
};

template<typename OfstreamIterator>
class VertexOrderFinderVisitor : public DfsVisitor<size_t, graph::Edge> {
public:
    explicit VertexOrderFinderVisitor(OfstreamIterator ofstream)
    : ofstream_(ofstream) {}
    
    void ExitVertex(size_t vertex_id) override {
        *ofstream_ = vertex_id;
        ++ofstream_;
    }
    
private:
    OfstreamIterator ofstream_;
};

class ComponentsBuilderVisitor : public DfsVisitor<size_t, graph::Edge> {
public:
    explicit ComponentsBuilderVisitor(size_t graph_size)
    : vertices_components_labels_(graph_size, -1) {}
    
    void DiscoverComponent() override {
        ++current_component_label_;
    }
    
    void DiscoverVertex(size_t vertex_id) override {
        vertices_components_labels_[vertex_id] = current_component_label_;
    }
    
    const std::vector<size_t>& GetComponentsList() const {
        return vertices_components_labels_;
    }
    
private:
    int current_component_label_ = -1;
    std::vector<size_t> vertices_components_labels_;
};

class InDegreeCounterVisitor : public DfsVisitor<size_t, graph::Edge> {
public:
    explicit InDegreeCounterVisitor(size_t graph_size)
    : in_degrees_(graph_size, 0) {}
    
    void ExamineEdge(const graph::Edge& edge) override {
        ++in_degrees_[edge.GetFinish()];
    }
    
    const std::vector<size_t>& GetInDegrees() const {
        return in_degrees_;
    }
private:
    std::vector<size_t> in_degrees_;
};

} // namespace visitors

namespace graph {
    
namespace traversals {

template<typename Visitor>
void DepthFirstSearch(const Graph& graph, typename Edge::Vertex origin_vertex,
                      std::unordered_set<typename Edge::Vertex>* visited_vertices,
                      Visitor* visitor)
{
    visitor->DiscoverVertex(origin_vertex);
    visited_vertices->insert(origin_vertex);
    for (const auto& edge : graph.GetEdges(origin_vertex)) {
        visitor->ExamineEdge(edge);
        auto finish = edge.GetFinish();
        if (!visited_vertices->count(finish)) {
            DepthFirstSearch(graph, finish, visited_vertices, visitor);
        }
    }
    visitor->ExitVertex(origin_vertex);
}


template<typename VerticesRangeIterator, typename Visitor>
void DepthFirstSearch(const Graph& graph,
                      VerticesRangeIterator vertices_range,
                      Visitor* visitor) {
    std::unordered_set<typename Edge::Vertex> visited_vertices;
    for (const auto& vertex : vertices_range) {
        if (!visited_vertices.count(vertex)) {
            visitor->DiscoverComponent();
            DepthFirstSearch(graph, vertex, &visited_vertices, visitor);
        }
    }
}

} // namespace traversals
    
using visitors::VertexOrderFinderVisitor;
using visitors::ComponentsBuilderVisitor;
using visitors::InDegreeCounterVisitor;

std::vector<size_t> LabelStronglyConnectedComponents(const Graph& graph) {
    std::vector<size_t> reversed_order_timeout(graph.GetVerticesNumber());
    
    VertexOrderFinderVisitor<std::vector<size_t>::iterator>
        vertex_order_visitor(reversed_order_timeout.begin());
    
    auto vertices_range = MakeRangeIterator(graph.GetVertices().begin(),
                                            graph.GetVertices().end());
    traversals::DepthFirstSearch(graph,
                                 vertices_range,
                                 &vertex_order_visitor);
    
    ComponentsBuilderVisitor components_builder_visitor(graph.GetVerticesNumber());
    auto reversed_vertices_range = MakeRangeIterator(reversed_order_timeout.rbegin(),
                                                     reversed_order_timeout.rend());
    
    traversals::DepthFirstSearch(CreateTransposeGraph(graph),
                                 reversed_vertices_range,
                                 &components_builder_visitor);
    return components_builder_visitor.GetComponentsList();
}

std::vector<size_t> FindVerticesInDegree(const Graph& graph) {
    InDegreeCounterVisitor in_degree_counter_visitor(graph.GetVerticesNumber());
    auto vertices_range = MakeRangeIterator(graph.GetVertices().begin(),
                                            graph.GetVertices().end());
    traversals::DepthFirstSearch(graph, vertices_range, &in_degree_counter_visitor);
    return in_degree_counter_visitor.GetInDegrees();
}

} // namespace graph

namespace game {

class GameResult {
public:
    enum class Result {
        kWinResult,
        kLossResult,
        kDrawResult
    };
    
    GameResult(size_t first_player, size_t second_player, Result result)
    : first_player_(first_player), second_player_(second_player),
      result_(result)
    {}
    
    const Result& GetResult() const {
        return result_;
    }
    
    size_t GetFirstPlayer() const {
        return first_player_;
    }
    
    size_t GetSecondPlayer() const {
        return second_player_;
    }
    
private:
    size_t first_player_, second_player_;
    Result result_;
};

graph::Graph CreateGameGraph(size_t num_people,
                             const std::vector<GameResult>& results) {
    graph::Graph game_results_graph(num_people);
    for (const auto& result : results) {
        if (result.GetResult() == GameResult::Result::kWinResult) {
            game_results_graph.AddEdge(graph::Edge(result.GetFirstPlayer(),
                                                   result.GetSecondPlayer()));
        } else if (result.GetResult() == GameResult::Result::kLossResult) {
            game_results_graph.AddEdge(graph::Edge(result.GetSecondPlayer(),
                                                   result.GetFirstPlayer()));
        }
    }
    return game_results_graph;
}

std::vector<GameResult> ReadGameResults(size_t num_games,
                                        std::istream& in_stream = std::cin) {
    std::vector<GameResult> results;
    results.reserve(num_games);
    for (size_t game_id = 0; game_id < num_games; ++game_id) {
        size_t first_player, second_player, result_id;
        in_stream >> first_player >> second_player >> result_id;
        --first_player;
        --second_player;
        
        GameResult::Result result;
        if (result_id == 1) {
            result = GameResult::Result::kWinResult;
        } else if (result_id == 2) {
            result = GameResult::Result::kLossResult;
        } else {
            result = GameResult::Result::kDrawResult;
        }
        results.push_back(GameResult(first_player, second_player, result));
    }
    return results;
}

} // namespace game


size_t FindMaxCompanySize(const graph::Graph& graph) {
    
    auto storngly_connected_components_id = graph::LabelStronglyConnectedComponents(graph);
    auto condencated_graph = graph::CreateCondenceGraph(graph, storngly_connected_components_id);
    auto in_degrees_in_condencated_graph = graph::FindVerticesInDegree(condencated_graph);
    
    std::vector<size_t> component_sizes(condencated_graph.GetVerticesNumber(), 0);
    for (size_t component_id : storngly_connected_components_id) {
        ++component_sizes[component_id];
    }
    
    size_t min_root_component_size = graph.GetVerticesNumber();
    
    for (const auto& component_vertex : condencated_graph.GetVertices()) {
        if (in_degrees_in_condencated_graph[component_vertex] == 0) {
            min_root_component_size = std::min(min_root_component_size,
                                               component_sizes[component_vertex]);
        }
    }
    return graph.GetVerticesNumber() - min_root_component_size + 1;
}

void ReadPeopleAndGamesNumber(size_t& num_people, size_t& num_games,
                              std::istream& in_stream = std::cin) {
    in_stream >> num_people >> num_games;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    size_t num_people = 0;
    size_t num_games = 0;
    ReadPeopleAndGamesNumber(num_people, num_games);
    std::vector<game::GameResult> game_results = game::ReadGameResults(num_games);
    std::cout << FindMaxCompanySize(game::CreateGameGraph(num_people, game_results));
    return 0;
}
