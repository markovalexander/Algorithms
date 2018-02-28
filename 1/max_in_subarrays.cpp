//
//  main.cpp
//  maxs_in_array
//
//  Created by Александр Марков on 14/09/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>

struct StackPair {
    int value;
    int current_max;
    
    const StackPair& operator=(const StackPair& other) {
        value = other.value;
        current_max = other.current_max;
        return *this;
    }
};

void push_pair(int value,
               std::stack<StackPair>* to_push) {
    int maximum = (to_push->empty() ?
                   value :
                   std::max(to_push->top().current_max, value)
                  );
    to_push->push({value, maximum});
}

void pop_pair(std::stack<StackPair>* to_push,
              std::stack<StackPair>* to_pop) {
    if (to_pop->empty()) {
        while (!to_push->empty()) {
            StackPair elem = to_push->top();
            int maximum = (to_pop->empty() ?
                           elem.value : std::max(to_pop->top().current_max, elem.value)
                          );
            to_pop->push({elem.value, maximum});
            to_push->pop();
        }
    }
    to_pop->pop();
}


int find_max(const std::stack<StackPair>& to_push,
             const std::stack<StackPair>& to_pop) {
    if (to_push.empty()) {
        return to_pop.top().current_max;
    }
    if (to_pop.empty()) {
        return to_push.top().current_max;
    }
    return std::max(to_push.top().current_max,
                    to_pop.top().current_max);
}

int left_operation(const std::vector<int>& elems,
                   std::stack<StackPair>* to_push,
                   std::stack<StackPair>* to_pop) {
    pop_pair(to_push, to_pop);
    int cur_max = find_max(*to_push, *to_pop);
    return cur_max;
}

int right_operation(const std::vector<int>& elems, int right_pos,
                    std::stack<StackPair>* to_push,
                    std::stack<StackPair>* to_pop) {
    push_pair(elems[right_pos], to_push);
    int cur_max = find_max(*to_push, *to_pop);
    return cur_max;
}

void print(const std::vector<int>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
}

int main(int argc, const char * argv[]) {
    std::stack<StackPair> to_push;
    std::stack<StackPair> to_pop;
    std::vector<int> elems;
    std::vector<int> results;
    size_t num_elems;
    size_t num_commands;
    int left_pos = 0;
    int right_pos = 0;
    
    std::cin >> num_elems;
    for (size_t i = 0; i < num_elems; ++i) {
        int elem;
        std::cin >> elem;
        elems.push_back(elem);
    }
    
    push_pair(elems[0], &to_push);
    std::cin >> num_commands;
    for (size_t i = 0; i < num_commands; ++i) {
        char command;
        std::cin >> command;
        if (command == 'L') {
            ++left_pos;
            results.push_back(left_operation(elems, &to_push, &to_pop));
        } else if (command == 'R') {
            ++right_pos;
            results.push_back(right_operation(elems, right_pos, &to_push, &to_pop));
        }
    }
    
    print(results);
    return 0;
}
