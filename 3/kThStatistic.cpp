//
//  main.cpp
//  kThStatiscs
//
//  Created by Александр Марков on 08/11/2017.
//  Copyright © 2017. All rights reserved.
//

#include <queue>
#include <memory>
#include <algorithm>
#include <vector>
#include <iostream>

class AVLtree {
    struct Node {
        int key;
        size_t height, 1node_count, entrance_count;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
    
        explicit Node(int key_)
        : key(key_), height(1), node_count(1), entrance_count(1),
        left(0), right(0)
        {}
    };
    
    using pointer = std::shared_ptr<Node>;
    
public:
    explicit AVLtree(int key) {
        Node node(key);
        head_ = std::make_shared<Node>(node);
    }
    
    void Insert(int key) {
        fix(head_);
        head_ = insert_(head_, key);
    }
    
    void Erase(int key) {
        fix(head_);
        head_ = remove_(head_, key);
    }
    
    int GetStat(size_t stat) {
        fix(head_);
        return get_stat(head_, stat);
    }
    
private:
    pointer head_;
    
    size_t tree_height(pointer node) {
        if (node != nullptr) {
            return node->height;
        } else {
            return 0;
        }
    }
    
    size_t nodes_amount(pointer node) {
        if (node != nullptr) {
            return node->node_count;
        } else {
            return 0;
        }
    }
    
    size_t entries_amount(pointer node) {
        if (node != nullptr) {
            return node->entrance_count;
        } else {
            return 0;
        }
    }
    
    int balance_factor(pointer node) {
        return static_cast<int> (tree_height(node->right) - tree_height(node->left));
    }
    
    void fix(pointer node) {
        size_t left_height = tree_height(node->left);
        size_t right_height = tree_height(node->right);
        
        node->height = std::max(left_height, right_height) + 1;
        node->node_count = nodes_amount(node->left)
                            + nodes_amount(node->right)
                            + node->entrance_count;
    }
    
    pointer left_rotate(pointer node) {
        pointer right = node->right;
        node->right = right->left;
        right->left = node;
        fix(node);
        fix(right);
        return right;
    }
    
    pointer right_rotate(pointer node) {
        pointer left = node->left;
        node->left = left->right;
        left->right = node;
        fix(node);
        fix(left);
        return left;
    }
    
    pointer balance(pointer node) {
        fix(node);
        if (balance_factor(node) == 2) {
            if (balance_factor(node->right) < 0) {
                node->right = right_rotate(node->right);
            }
            return left_rotate(node);
        }
        
        if (balance_factor(node) == -2) {
            if (balance_factor(node->left) > 0) {
                node->left = left_rotate(node->left);
            }
            return right_rotate(node);
        }
        return node;
    }
    
    pointer find_min(pointer node) {
        if (node->left != nullptr) {
            return find_min(node->left);
        } else {
            return node;
        }
    }
    
    pointer extract_min(pointer node) {
        if (node->left == nullptr) {
            return node->right;
        }
        node->left = extract_min(node->left);
        return balance(node);
    }
    
    pointer remove_(pointer node, int value) {
        if (node == nullptr) {
            return nullptr;
        }
        if (value < node->key) {
            node->left = remove_(node->left, value);
        } else if (value > node->key) {
            node->right = remove_(node->right, value);
        } else if (value == node->key) {
            node->entrance_count--;
            if (node->entrance_count == 0) {
                pointer left = node->left;
                pointer right = node->right;
                
                if (right == nullptr) {
                    return left;
                }
                if (left == nullptr) {
                    return right;
                }
                pointer min_node = find_min(right);
                min_node->right = extract_min(right);
                min_node->left = left;
                return balance(min_node);
            }
        }
        return balance(node);
    }
    
    pointer insert_(pointer node, int value) {
        if (node == nullptr) {
            Node new_node(value);
            return std::make_shared<Node>(new_node);
        }
        if (node->key > value) {
            node->left = insert_(node->left, value);
        }
        if (node->key < value) {
            node->right = insert_(node->right, value);
        }
        if (node->key == value) {
            node->entrance_count++;
            node = balance(node);
        }
        return balance(node);
    }
    
    pointer find_(pointer node, int value) {
        if (node == nullptr) {
            return nullptr;
        }
        if (node->key < value) {
            return find_(node->right, value);
        }
        if (node->key > value) {
            return find_(node->left, value);
        }
        if (node->key == value) {
            return node;
        }
        return nullptr;
    }

    int get_stat(pointer node, size_t stat) {
        if (node == nullptr) {
            return -1;
        }
        if (node->left != nullptr) {
            if (nodes_amount(node->left) < stat
                && nodes_amount(node->left) + node->entrance_count >= stat) {
                return node->key;
            }
            if (nodes_amount(node->left) >= stat) {
                return get_stat(node->left, stat);
            }
            if (nodes_amount(node->left) + node->entrance_count < stat) {
                return get_stat(node->right,
                                stat - nodes_amount(node->left) - node->entrance_count);
            }
        } else {
            if (stat <= node->entrance_count) {
                return node->key;
            } else {
                return get_stat(node->right, stat - node->entrance_count);
            }
        }
        return -1;
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    std::vector<int> numbers;
    size_t n_numbers, n_queries, stat;
    std::cin >> n_numbers >> n_queries >> stat;
    for (size_t i = 0; i < n_numbers; ++i) {
        int value;
        std::cin >> value;
        numbers.push_back(value);
    }
    std::vector<int> answers;
    size_t left{0}, right{1};

    AVLtree tree(numbers[0]);
    for (size_t iteration = 0; iteration < n_queries; ++iteration) {
        char ch;
        std::cin >> ch;
        if (ch == 'R') {
            int value = numbers[right++];
            tree.Insert(value);
            answers.push_back(tree.GetStat(stat));
        }
        if (ch == 'L') {
            int value = numbers[left++];
            tree.Erase(value);
            answers.push_back(tree.GetStat(stat));
        }
    }
    
    for (int st: answers) {
        std::cout << st << "\n";
    }
    return 0;
}

