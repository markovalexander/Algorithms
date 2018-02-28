//
//  main.cpp
//  SegmentTree
//
//  Created by Александр Марков on 16/12/2017.
//  Copyright © 2017. All rights reserved.
//

#include <algorithm>
#include <iostream>
#include <vector>
#include <memory>

struct Segment {
    int64_t begin;
    int64_t end;
};

constexpr int64_t kINFSegment = -1;

class SegmentsTree {
    
    struct tree_node {
        Segment segment;
        int64_t real_length;
        int64_t covering_segments_count;
        int64_t covering_length;
        int64_t type;
        
        tree_node(Segment segment_, int64_t type_ = 1)
        : segment(segment_), real_length(segment.end - segment.begin),
        covering_segments_count(0), covering_length(0), type(type_)
        {}
    };
    
public:
    SegmentsTree() {}
    
    explicit SegmentsTree(const std::vector<int64_t> segment_ends) {
        size_t tree_size_ = 1 << (log_(static_cast<int>(segment_ends.size()) - 1));
        tree_.resize(2 * tree_size_ - 1, tree_node({0, 0}, kINFSegment));
        
        std::vector<tree_node> initial_nodes = getInitialNodes(segment_ends);
        for (size_t i = 0; i < initial_nodes.size(); ++i) {
            tree_[i + tree_size_ - 1] = initial_nodes[i];
        }
        
        for (int64_t i = static_cast<int>(tree_size_) - 2; i >= 0; --i) {
            tree_[i] = buildUpperNode(tree_[2 * i + 1], tree_[2 * i + 2]);
        }
    }
    
    size_t coverLength()  {
        return tree_[0].covering_length;
    }
    
    void Erase(Segment segment) {
        update_all(0, segment, -1);
    }
    
    void Insert(Segment segment) {
        update_all(0, segment, 1);
    }
    
private:
    std::vector<tree_node> tree_;
    
    size_t log_(int64_t number) {
        size_t ans = 0;
        while (number /= 2)
            ++ans;
        return ans + 1;
    }
    
    std::vector<tree_node> getInitialNodes(std::vector<int64_t> segment_ends) {
        std::vector<tree_node> nodes;
        for (size_t i = 0; i + 1 < segment_ends.size(); ++i) {
            Segment segment{segment_ends[i], segment_ends[i + 1]};
            nodes.emplace_back(segment);
        }
        return nodes;
    }
    
    tree_node buildUpperNode(tree_node left, tree_node right) {
        if (left.type == kINFSegment) {
            return tree_node({0, 0}, kINFSegment);
        }
        if (right.type == kINFSegment) {
            return left;
        }
        return tree_node({left.segment.begin, right.segment.end}, 1);
    }
    
    int64_t getRealCoveringLength(size_t pos) {
        if (pos >= tree_.size()) {
            return 0;
        }
        
        if (tree_[pos].type == -1) {
            return 0;
        }
        return tree_[pos].covering_length;
    }
    
    void update_all(size_t pos, Segment segment, int64_t delta) {
        if (pos >= tree_.size()) {
            return;
        }
        
        if (tree_[pos].segment.begin >= segment.end || tree_[pos].segment.end <= segment.begin) {
            return;
        }
        
        if (tree_[pos].segment.begin >= segment.begin && tree_[pos].segment.end <= segment.end) {
            tree_[pos].covering_segments_count += delta;
            if (tree_[pos].covering_segments_count > 0) {
                tree_[pos].covering_length = tree_[pos].real_length;
                return;
            } else {
                tree_[pos].covering_length = getRealCoveringLength(2 * pos + 1)
                                             + getRealCoveringLength(2 * pos + 2);
                return;
            }
        }
        
        update_all(2 * pos + 1, segment, delta);
        update_all(2 * pos + 2, segment, delta);
        if (tree_[pos].covering_segments_count > 0) {
            tree_[pos].covering_length = tree_[pos].real_length;
        } else {
            tree_[pos].covering_length = getRealCoveringLength(2 * pos + 1)
                                         + getRealCoveringLength(2 * pos + 2);
        }
    }
};

struct Query {
    int type;
    Segment segment;
};

int main() {
    size_t n_queries;
    std::cin >> n_queries;
    std::vector<Query> queries;
    std::vector<int64_t> numbers;
    int64_t begin, end;
    
    char c;
    for (size_t i = 0; i < n_queries; ++i) {
        std::cin >> c >> begin >> end;
        if (c == '+') {
            queries.push_back({1, {begin, end}});
            numbers.push_back(begin);
            numbers.push_back(end);
        } else {
            queries.push_back({-1, {begin, end}});
        }
    }
    
    std::sort(numbers.begin(), numbers.end());
    
    SegmentsTree tree(numbers);
    std::vector<int64_t> answers;
    
    for (const auto& query : queries) {
        if (query.type == -1) {
            tree.Erase(query.segment);
        } else {
            tree.Insert(query.segment);
        }
        answers.push_back(tree.coverLength());
    }

    for (auto answer : answers) {
        std::cout << answer << "\n";
    }
    
    return 0;
}
