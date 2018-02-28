//
//  main.cpp
//  BST_traversals
//
//  Created by Александр Марков on 28/11/2017.
//  Copyright © 2017. All rights reserved.
//

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

class BST {
    struct vertex {
        int value;
        std::shared_ptr<vertex> left, right;
        
        explicit vertex(int value_)
        : value(value_),
        left(nullptr), right(nullptr) {}
        
        vertex(int value_, std::shared_ptr<vertex> left_,
                         std::shared_ptr<vertex> right_)
        : value(value_),
        left(left_), right(right_) {}
    };
    
    using pointer = std::shared_ptr<vertex>;
    
public:
    BST() {}
    
    void Initialize(const std::vector<int>& pre_order) {
        if (pre_order.size() == 0) {
            return;
        }
        head_ = std::make_shared<vertex>(pre_order[0]);
        pointer last_node = head_;
        std::vector<pointer> no_right_child;
        no_right_child.push_back(head_);
        for (size_t index = 1; index < pre_order.size(); ++index) {
            pointer node_to_add = std::make_shared<vertex>(pre_order[index]);
            if (last_node->value > node_to_add->value) {
                last_node->left = node_to_add;
                last_node = node_to_add;
                no_right_child.push_back(node_to_add);
            } else {
                while (no_right_child.size() > 0
                       && no_right_child.back()->value <= node_to_add->value)
                {
                    last_node = no_right_child.back();
                    no_right_child.pop_back();
                }
                
                last_node->right = node_to_add;
                last_node = node_to_add;
                no_right_child.push_back(node_to_add);
            }
        }
        
        create_inorder(head_);
        create_postorder(head_);
    }
    
    const std::vector<int> GetInOrder() const {
        return in_order_;
    }
    
    const std::vector<int> GetPostOrder() const {
        return post_order_;
    }
    
private:
    pointer head_;
    std::vector<int> pre_order_, in_order_, post_order_;
    
    void create_inorder(pointer node) {
        if (node == nullptr) {
            return;
        }
        create_inorder(node->left);
        in_order_.push_back(node->value);
        create_inorder(node->right);
    }
    
    void create_postorder(pointer node) {
        if (node == nullptr) {
            return;
        }
        create_postorder(node->left);
        create_postorder(node->right);
        post_order_.push_back(node->value);
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    BST tree;
    size_t n_nodes;
    std::cin >> n_nodes;
    
    std::vector<int> pre_order;
    for (size_t i = 0; i < n_nodes; ++i) {
        int value;
        std::cin >> value;
        pre_order.push_back(value);
    }
    
    tree.Initialize(pre_order);
    std::vector<int> in_order = tree.GetInOrder();
    std::vector<int> post_order = tree.GetPostOrder();
    
    for (int value : post_order) {
        std::cout << value << " ";
    }
    std::cout << "\n";
    
    for (int value : in_order) {
        std::cout << value << " ";
    }
    return 0;
}
