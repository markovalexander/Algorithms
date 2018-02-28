//
//  main.cpp
//  LongestCommonSubseq
//
//  Created by Александр Марков on 08/10/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <vector>

using i_vector = std::vector<int>;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    size_t num_first, num_second;
    
    std::cin >> num_first;
    i_vector first_sequence(num_first);
    for (size_t i = 0; i < num_first; ++i) {
        std::cin >> first_sequence[i];
    }
    
    std::cin >> num_second;
    i_vector second_sequence(num_second);
    for (size_t i = 0; i < num_second; ++i) {
        std::cin >> second_sequence[i];
    }
    
    std::vector<i_vector> table;
    for (size_t i = 0; i < num_second + 1; ++i) {
        i_vector line(num_first + 1, 0);
        table.push_back(line);
    }
    
    for (size_t j = 1; j < num_second + 1; ++j) {
        for (size_t i = 1; i < num_first + 1; ++i) {
            if (first_sequence[i - 1] == second_sequence[j - 1]) {
                table[j][i] = table[j - 1][i - 1] + 1;
            } else {
                table[j][i] = std::max(table[j - 1][i], table[j][i - 1]);
            }
        }
    }
    std::cout << table[num_second][num_first];
    return 0;
}
