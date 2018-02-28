    //
    //  main.cpp
    //  SortedSequencesMerging
    //
    //  Created by Александр Марков on 08/10/2017.
    //  Copyright © 2017. All rights reserved.
    //

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <list>

class THeappy {
public:
    THeappy()
    : index_(0)
    {}
    
    explicit THeappy(const std::vector<int>& numbers)
    : index_(0), elems_(numbers)
    {}
    
    const bool HasElems() const {
        return !(index_ == elems_.size());
    }
    
    const int GetElem() {
        return elems_[index_++];
    }
    
    const size_t GetIndex() const {
        return index_;
    }
    
private:
    size_t index_;
    std::vector<int> elems_;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    size_t n_heaps, n_elems;
    std::cin >> n_heaps >> n_elems;
    
    std::vector<THeappy> heappy;
    for (size_t i = 0; i < n_heaps; ++i) {
        std::vector<int> numbers;
        for (size_t j = 0; j < n_elems; ++j) {
            int value;
            std::cin >> value;
            numbers.push_back(value);
        }
        heappy.push_back(THeappy(numbers));
    }
    
    using si_pair = std::pair<int, size_t>;
    std::vector<si_pair> heap;
    for (size_t i = 0; i < n_heaps; ++i) {
        heap.push_back({heappy[i].GetElem(), i});
    }
    
    std::vector<int> answer;
    std::make_heap(heap.begin(), heap.end(),
               [&] (si_pair first_p, si_pair second_p) {return first_p.first > second_p.first;});
    while (heap.size() > 0) {
        std::pop_heap(heap.begin(), heap.end(),
               [&] (si_pair first_p, si_pair second_p) {return first_p.first > second_p.first;});
        auto largest = heap.back();
        heap.pop_back();
        answer.push_back(largest.first);
        if (heappy[largest.second].HasElems()) {
            heap.push_back({heappy[largest.second].GetElem(), largest.second });
            std::push_heap(heap.begin(), heap.end(),
               [&] (si_pair first_p, si_pair second_p) {return first_p.first > second_p.first;});
        }
    }
    
    for (int elem: answer) {
        std::cout << elem << " ";
    }
    
    return 0;
}


