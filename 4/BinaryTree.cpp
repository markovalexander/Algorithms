//
//  main.cpp
//  BinaryTreesNumber
//
//  Created by Александр Марков on 29/11/2017.
//  Copyright © 2017. All rights reserved.
//

#include <algorithm>
#include <iostream>
#include <vector>

constexpr int32_t kModulo = 123456789;

int FindNumberOfTrees(const std::vector<int32_t>& numbers) {
    std::vector<int32_t> sorted_numbers(numbers);
    std::sort(sorted_numbers.begin(), sorted_numbers.end());
    std::vector<std::vector<int32_t>> dynamics;
    for (size_t i = 0; i < numbers.size(); ++i) {
        std::vector<int> begin_state(numbers.size(), 1);
        dynamics.push_back(std::move(begin_state));
    }
    for (size_t sub_array_len = 1; sub_array_len < numbers.size(); ++sub_array_len) {
        for (size_t begin = 0; begin + sub_array_len < numbers.size(); ++begin) {
            size_t end = begin + sub_array_len;
            dynamics[begin][end] = dynamics[begin + 1][end];
            if (sorted_numbers[end] != sorted_numbers[end - 1]) {
                dynamics[begin][end] += dynamics[begin][end - 1];
                dynamics[begin][end] %= kModulo;
            }
            for (size_t root_index = begin + 1; root_index < end; ++root_index) {
                if (sorted_numbers[root_index] != sorted_numbers[root_index - 1]) {
                    dynamics[begin][end] += (dynamics[begin][root_index - 1]
                                             * dynamics[root_index + 1][end]);
                    dynamics[begin][end] %= kModulo;
                }
            }
        }
    }
    return dynamics[0][numbers.size() - 1];
}


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    size_t n_keys = 0;
    std::vector<int32_t> numbers;
    
    std::cin >> n_keys;
    for (size_t i = 0; i < n_keys; ++i) {
        int32_t number;
        std::cin >> number;
        numbers.push_back(number);
    }
    
    int answer = FindNumberOfTrees(numbers);
    std::cout << answer;
    return 0;
}
