//
//  main.cpp
//  ImplicitKeyTreap
//
//  Created by Александр Марков on 16/12/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include <memory>


using std::string;

struct shift {
    size_t start, finish, n_times;
    
    shift(size_t s_, size_t f_, size_t t_)
    : start(s_), finish(f_), n_times(t_) {}
};

template<typename T>
class ImplicitKeyTreap {
    
    struct tree_node {
        // string letter;
        T value;
        int64_t priority;
        size_t size;
        
        std::shared_ptr<tree_node> left, right;
        
        tree_node(T value_, int64_t priority_)
        : value(value_), priority(priority_),
          size(1),
          left(nullptr), right(nullptr)
        {}
    };
    
    using pointer = std::shared_ptr<tree_node>;
    using pair_pointer = std::pair<pointer, pointer>;
    
public:
    ImplicitKeyTreap()
    : head_(nullptr) {}
    
    void Add(size_t pos, T value) {
        pair_pointer split_res = split(head_, pos);
        
        pointer to_add = std::make_shared<tree_node>(value, generator());
        pointer merge_result = merge(split_res.first, to_add);
        head_ = merge(merge_result, split_res.second);
    }
    
    void CycleShift(shift shift_p) {
        pair_pointer first_split = split(head_, shift_p.finish);
        pair_pointer second_split = split(first_split.first, shift_p.start);
        
        pointer left = second_split.first;
        pointer interval = second_split.second;
        pointer right = first_split.second;
        
        pair_pointer interval_split = split(interval,
                                            shift_p.n_times);
        interval = merge(interval_split.second, interval_split.first);
        left = merge(left, interval);
        head_ = merge(left, right);
    }
    
    void Print() const {
        in_order_print(head_);
    }
    
    size_t Size() const {
        return node_size(head_);
    }
    
private:
    pointer head_;
    std::mt19937 generator;
    
    void in_order_print(pointer node) const {
        if (node == nullptr) {
            return;
        }
        in_order_print(node->left);
        std::cout << node->value;
        in_order_print(node->right);
    }
    
    size_t node_size(pointer node) const {
        if (node == nullptr) {
            return 0;
        }
        return node->size;
    }
    
    void recalc(pointer node) {
        if (node == nullptr) {
            return;
        }
        node->size = 1 + node_size(node->left) + node_size(node->right);
    }
    
    pointer merge(pointer left, pointer right) {
        if (left == nullptr) {
            return right;
        }
        if (right == nullptr) {
            return left;
        }
        if (left->priority > right->priority) {
            left->right = merge(left->right, right);
            recalc(left);
            return left;
        } else {
            right->left = merge(left, right->left);
            recalc(right);
            return right;
        }
        return nullptr;
    }
    
    pair_pointer split(pointer node, size_t pos) {
        if (node == nullptr) {
            return {nullptr, nullptr};
        }
        
        size_t left_size = node_size(node->left);
        if (left_size == pos) {
            pointer left_copy = node->left;
            node->left = nullptr;
            recalc(node);
            return {left_copy, node};
        } else if (left_size > pos) {
            pair_pointer sub_split_res = split(node->left, pos);
            node->left = sub_split_res.second;
            recalc(node);
            return {sub_split_res.first, node};
        } else {
            pair_pointer sub_split_res = split(node->right, pos - left_size - 1);
            node->right = sub_split_res.first;
            recalc(node);
            return {node, sub_split_res.second};
        }
    }
};

int main() {
    ImplicitKeyTreap<std::string> treap;
    string string_;
    size_t n_shift;
    
    std::cin >> string_;
    std::cin >> n_shift;
    size_t start, finish, t_shift;
    std::vector<shift> shifts;
    
    for (auto letter: string_) {
        treap.Add(treap.Size(), std::string(1, letter));
    }
    
    for (size_t i = 0; i < n_shift; ++i) {
        std::cin >> start >> finish >> t_shift;
        shifts.push_back({--start, finish, t_shift});
    }
    
    for (auto shifty = shifts.rbegin(); shifty != shifts.rend(); ++shifty) {
        treap.CycleShift(*shifty);
    }
    
    treap.Print();
    return 0;
}
