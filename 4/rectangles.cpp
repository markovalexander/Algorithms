//
//  main.cpp
//  Rectangles
//
//  Created by Александр Марков on 28/11/2017.
//  Copyright © 2017. All rights reserved.
//

#include <algorithm>
#include <iostream>
#include <vector>
#include <memory>

struct vertical_line {
    int begin, end;
    int x_coord;
    int type;
    
    vertical_line(int b_, int e_, int x_coord_, int type_)
    : begin(b_), end(e_),
      x_coord(x_coord_),
      type(type_)
    { }
    
    vertical_line(const vertical_line& other) = default;
    
    const bool operator==(const vertical_line& other) const {
        return begin == other.begin && end == other.end;
    }
};

bool CompareLinesInSort(const vertical_line& first, const vertical_line& second) {
    return first.x_coord < second.x_coord;
}

bool CompareLinesInTree(const vertical_line& first, const vertical_line& second) {
    return first.end < second.end;
}

class LinesSet {
    struct vertex {
        vertical_line key;
        std::shared_ptr<vertex> left, right;
        size_t node_count, height;
        
        explicit vertex(const vertical_line& key_)
        : key(key_), node_count(1),
          height(1)
        { }
    };
    
    using pointer = std::shared_ptr<vertex>;
    
public:
    LinesSet() {}
    
    void Insert(const vertical_line& key) {
        if (head_ != nullptr) {
            fix(head_);
        }
        head_ = insert_(head_, key);
    }
    
    void Erase(const vertical_line& key) {
        if (head_ != nullptr) {
            fix(head_);
        }
        head_ = remove_(head_, key);
    }
    
    bool DoesCover(const vertical_line& line)  {
        if (head_ != nullptr) {
            fix(head_);
        }
        pointer try_line = nullptr;
        lower_bound_(head_, line, &try_line);
        if (try_line == nullptr) {
            return false;
        }
        if (try_line->key.begin < line.begin
            && try_line->key.end > line.end)
        {
            return true;
        } else {
            return false;
        }
        return false;
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
    
    int balance_factor(pointer node) {
        return static_cast<int> (tree_height(node->right) - tree_height(node->left));
    }
    
    void fix(pointer node) {
        size_t left_height = tree_height(node->left);
        size_t right_height = tree_height(node->right);
        
        node->height = std::max(left_height, right_height) + 1;
        node->node_count = nodes_amount(node->left) + nodes_amount(node->right) + 1;
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
    
    pointer find_max(pointer node) {
        if (node->right != nullptr) {
            return find_max(node->right);
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
    
    void lower_bound_(pointer node, const vertical_line& line, pointer* current_l_b) {
        if (node == nullptr) {
            return;
        }
        if (CompareLinesInTree(node->key, line)) {
            lower_bound_(node->right, line, current_l_b);
        } else {
            *current_l_b = node;
            lower_bound_(node->left, line, current_l_b);
        }
    }
    
    pointer remove_(pointer node, const vertical_line& line) {
        if (node == nullptr) {
            return nullptr;
        }
        if (CompareLinesInTree(line, node->key)) {
            node->left = remove_(node->left, line);
        } else if (CompareLinesInTree(node->key, line)) {
            node->right = remove_(node->right, line);
        } else if (line == node->key) {
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
        return balance(node);
    }
    
    pointer insert_(pointer node, const vertical_line& line) {
        if (node == nullptr) {
            vertex new_node(line);
            return std::make_shared<vertex>(new_node);
        }
        if (CompareLinesInTree(line, node->key)) {
            node->left = insert_(node->left, line);
        }
        if (CompareLinesInTree(node->key, line)) {
            node->right = insert_(node->right, line);
        }
        return balance(node);
    }
    
    pointer find_(pointer node, const vertical_line& line) {
        if (node == nullptr) {
            return nullptr;
        }
        if (CompareLinesInTree(node->key, line)) {
            return find_(node->right, line);
        }
        if (CompareLinesInTree(line, node->key)) {
            return find_(node->left, line);
        }
        if (node->key == line) {
            return node;
        }
        return nullptr;
    }
};

size_t FindNumberOfExternalRectangles(const std::vector<vertical_line>& lines,
                                      LinesSet* tree) {
    size_t counter = 1;
    
    tree->Insert(lines[0]);
    for (size_t iter = 1; iter < lines.size(); ++iter) {
        vertical_line line = lines[iter];
        int type = line.type;
        if (type == 1 && !tree->DoesCover(line)) {
            ++counter;
            tree->Insert(line);
        }
        if (type == -1) {
            tree->Erase(line);
        }
    }
    return counter;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    size_t n_rectangles;
    std::cin >> n_rectangles;
    std::vector<vertical_line> lines;
    
    for (size_t iter = 0; iter < n_rectangles; ++iter) {
        int f_x, f_y, s_x, s_y;
        std::cin >> f_x >> f_y >> s_x >> s_y;
        vertical_line open(std::min(f_y, s_y),
                           std::max(f_y, s_y),
                           std::min(f_x, s_x),
                           1);
        
        vertical_line close(std::min(f_y, s_y),
                            std::max(f_y, s_y),
                            std::max(f_x, s_x),
                            -1);
        
        lines.push_back(open);
        lines.push_back(close);
    }
    
    std::sort(lines.begin(), lines.end(), CompareLinesInSort);
    LinesSet line_set;
    size_t answer = FindNumberOfExternalRectangles(lines, &line_set);
    std::cout << answer;
    
    return 0;
}
