//
//  main.cpp
//  embedded_cubes
//
//  Created by Александр Марков on 14/09/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>


struct Cube {
    int first_dis;
    int second_dis;
    int third_dis;
    
    bool operator<(const Cube& other) {
        return (first_dis <other.first_dis) && (second_dis < other.second_dis)
                    && (third_dis < other.third_dis);
    }
};

bool operator<(const Cube& first_cube, const Cube& second_cube) {
    return (first_cube.first_dis < second_cube.first_dis)
                && (first_cube.second_dis < second_cube.second_dis)
                && (first_cube.third_dis < second_cube.third_dis);
}

int main(int argc, const char * argv[]) {
    std::vector<Cube> cubes;
    size_t num_cubes;
    
    std::cin >> num_cubes;
    for (size_t i = 0; i < num_cubes; ++i) {
        int f_dis, s_dis, th_dis;
        std::cin >> f_dis >> s_dis >> th_dis;
        cubes.push_back({f_dis, s_dis, th_dis});
    }
    
    std::vector<int> max_increasing_subseq(num_cubes, 1);
    for (size_t i = 0; i < num_cubes; ++i) {
        for (size_t j = 0; j < i; ++j) {
            if (cubes[j] < cubes[i]) {
                max_increasing_subseq[i] = std::max(max_increasing_subseq[i],
                                                 max_increasing_subseq[j] + 1);
            }
        }
    }
    
    size_t ans = *std::max_element(max_increasing_subseq.begin(), max_increasing_subseq.end());
    std::cout << ans;
    
    return 0;
}
