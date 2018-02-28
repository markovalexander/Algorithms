//
//  main.cpp
//  Triangles
//
//  Created by Александр Марков on 31/10/2017.
//  Copyright © 2017. All rights reserved.
//

#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <list>

constexpr int kPrimeModule = 113;
constexpr int kMaxValue = kPrimeModule * kPrimeModule * kPrimeModule + 3;

class Triangle {
public:
    explicit Triangle(std::vector<int> sides)
    : sides_{sides}
    {
        normalize();
        setHash();
    }
    
    Triangle(int f_side, int s_side, int t_side)
    : sides_{{f_side, s_side, t_side}}
    {
        normalize();
        setHash();
    }
    
    Triangle(const Triangle& other)
    : sides_(other.sides_), hash_(other.hash_)
    {}
    
    Triangle(Triangle&&) = default;
    
    const int getHash() const {
        return hash_;
    }
    
    const std::vector<int> getSides() const {
        return sides_;
    }
    
    const bool operator==(const Triangle& other) const {
        return sides_ == other.sides_;
    }

private:
    std::vector<int> sides_;
    int hash_;
    
    void normalize() {
        int gcd = gcd_(sides_[0], gcd_(sides_[1], sides_[2]));
        for (size_t i = 0; i < 3; ++i) {
            sides_[i] /= gcd;
        }
        std::sort(sides_.begin(), sides_.end(), std::less<>());
    }
    
    void setHash() {
        int first_ = sides_[2];
        int second_ = sides_[1];
        int third_ = sides_[0];
        hash_ = (first_ % kMaxValue * kPrimeModule
                + second_) % kMaxValue * kPrimeModule
                + third_;
        hash_ = hash_ % kMaxValue;
    }
    
    int gcd_(int first, int second) {
        while (second) {
            first %= second;
            std::swap(first, second);
        }
        return first;
    }
};

int solve(const std::vector<Triangle>& triangles) {
    int count_labels{0};
    std::vector<std::list<Triangle>> labeld_triangles(kMaxValue);
    std::vector<int> labels(kMaxValue, -1);
    
    for (const Triangle& triangle : triangles) {
        bool should_add = true;
        int hash = triangle.getHash();
        if (labels[hash] == -1) {
            labels[hash] = 1;
            should_add = true;
        } else {
            for (const Triangle& hash_triangle: labeld_triangles[hash]) {
                if (triangle == hash_triangle) {
                    should_add = false;
                    break;
                }
            }
        }
        if (should_add) {
            labeld_triangles[hash].push_front(triangle);
            ++count_labels;
        }
    }
    return count_labels;
}


void makeRandom(std::vector<Triangle>* triangles) {
    int a_side;
    int b_side;
    int c_side;
    size_t triangles_count = 1000000;
    for (size_t i = 0; i < triangles_count; ++i) {
        a_side = rand() % 1000000000 + kPrimeModule;
        b_side = rand() % 1000000000 + kPrimeModule;
        c_side = rand() % 1000000000 + kPrimeModule;
        triangles->push_back({a_side, b_side, c_side});
    }
}


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n_triangles;
    std::cin >> n_triangles;
    std::vector<Triangle> triangles;
    
    for (size_t i = 0; i < n_triangles; ++i) {
        int f_side, s_side, t_side;
        std::cin >> f_side >> s_side >> t_side;
        triangles.push_back({f_side, s_side, t_side});
    }
    
    int ans = solve(triangles);
    std::cout << ans;
    return 0;
}
