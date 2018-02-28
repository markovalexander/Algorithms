//
//  main.cpp
//  DSU
//
//  Created by Александр Марков on 17/12/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <numeric>
#include <string>
#include <sstream>
#include <random>
#include <vector>

class DSU {
public:
    DSU() {}
    
    explicit DSU(size_t dsu_size) {
        parents_.resize(dsu_size);
        std::iota(parents_.begin(), parents_.end(), 0);
    }
    
    void Add() {
        parents_.push_back(parents_.size());
    }
    
    size_t Find(size_t index) {
        if (parents_[index] == index) {
            return index;
        } else {
            return parents_[index] = Find(parents_[index]);
        }
    }
    
    void Unite(size_t first_set_elem, size_t second_set_elem) {
        size_t set_index = generator() % 2;
        size_t first_set_index = Find(first_set_elem);
        size_t second_set_index = Find(second_set_elem);
        
        if (set_index == 0) {
            parents_[second_set_index] = first_set_index;
        } else {
            parents_[first_set_index] = second_set_index;
        }
    }
    
    size_t SetDirector(size_t director, size_t subject) {
        size_t subject_director = Find(subject);
        size_t directors_director = Find(director);
        
        if (subject == subject_director
            && directors_director != subject_director)
        {
            parents_[subject] = director;
            return 1;
        } else {
            return 0;
        }
        return 0;
    }
    
private:
    std::vector<size_t> parents_;
    std::default_random_engine generator;
};

struct Query {
    int type;
    size_t first;
    size_t second;
};

std::vector<Query> ReadQueries() {
    size_t n_queries;
    std::cin >> n_queries;
    std::vector<Query> queries;
    
    std::string str;
    std::getline(std::cin, str);
    
    for (size_t iter = 0; iter < n_queries; ++iter) {
        std::string str;
        std::getline(std::cin, str);
        std::stringstream ss;
        ss << str;
        size_t number;
        std::vector<size_t> numbers;
        
        while (ss >> number) {
            numbers.push_back(--number);
        }
        
        if (numbers.size() == 1) {
            queries.push_back({1, numbers[0], 0});
        } else {
            queries.push_back({2, numbers[0], numbers[1]});
        }
    }
    
    return queries;
}

std::vector<size_t> EmulateQueries(size_t n_elems, const std::vector<Query>& queries) {
    DSU dsu(n_elems);
    std::vector<size_t> answers;
    answers.reserve(queries.size());
    for (const auto& query : queries) {
        if (query.type == 1) {
            answers.push_back(dsu.Find(query.first) + 1);
        } else {
            answers.push_back(dsu.SetDirector(query.first, query.second));
        }
    }
    return answers;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie();
    
    size_t n_people;
    std::cin >> n_people;
    std::vector<Query> queries = ReadQueries();
    std::vector<size_t> answers = EmulateQueries(n_people, queries);
    for (auto answer : answers) {
        std::cout << answer << "\n";
    }
    return 0;
}
