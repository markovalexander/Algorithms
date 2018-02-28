//
//  main.cpp
//  longest_changing_subseq
//
//  Created by Александр Марков on 14/09/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>

constexpr int kNoSequence = -1;

struct SSequenceElem {
    size_t length;
    int parent = kNoSequence;
    
    SSequenceElem(size_t _l, int _p)
        : length(_l),
        parent(_p) { }
    
    const bool operator<(const SSequenceElem& other) const {
        return length < other.length;
    }
    
    const bool operator==(const SSequenceElem& other) const {
        return length == other.length;
    }
    
    const size_t getLength() const {
        return length;
    }
    
    const int getParent() const {
        return parent;
    }
    
    void setLength(size_t _l) {
        length = _l;
    }
    
    void setParent(int _p) {
        parent = _p;
    }
};

using SeqVector = std::vector<SSequenceElem>;

void CreateSubsequences(const std::vector<int>& numbers,
                        SeqVector* greater,
                        SeqVector* smaller) {
    int size = static_cast<int>(numbers.size());
    for (int current = size - 1; current >= 0; --current) {
        for (size_t check = current + 1; check < numbers.size(); ++check) {
            if (numbers[check] < numbers[current]
                && (*greater)[check].getLength() >= (*smaller)[current].getLength()) {
                (*smaller)[current].setLength((*greater)[check].getLength() + 1);
                (*smaller)[current].setParent(static_cast<int>(check));
            }
            if (numbers[check] > numbers[current]
                && (*smaller)[check].getLength() >= (*greater)[current].getLength()) {
                (*greater)[current].setLength((*smaller)[check].getLength() + 1);
                (*greater)[current].setParent(static_cast<int>(check));
            }
        }
    }
}

void RestoreAnswer(const SeqVector& greater,
                   const SeqVector& smaller,
                   const std::vector<int>& numbers) {
    size_t max_in_greater = std::max_element(greater.begin(), greater.end()) - greater.begin();
    size_t max_in_smaller = std::max_element(smaller.begin(), smaller.end()) - smaller.begin();
    
    int start{0};
    bool great_start = false;
    if (smaller[max_in_smaller].getLength() > greater[max_in_greater].getLength()) {
        start = static_cast<int>(max_in_smaller);
    } else if (greater[max_in_greater].getLength() > smaller[max_in_smaller].getLength()) {
        start = static_cast<int>(max_in_greater);
        great_start = true;
    } else if (max_in_greater > max_in_smaller) {
        start = static_cast<int>(max_in_smaller);
    } else if (max_in_greater < max_in_smaller) {
        start = static_cast<int>(max_in_greater);
        great_start = true;
    } else if (greater[max_in_greater].getParent() < smaller[max_in_smaller].getParent()) {
        great_start = true;
    }
    
    std::vector<int> answer;
    while (start != kNoSequence) {
        answer.push_back(numbers[start]);
        if (great_start) {
            start = greater[start].getParent();
        } else {
            start = smaller[start].getParent();
        }
        great_start = !great_start;
    }
    
    for (int value : answer) {
        std::cout << value << " ";
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::size_t number_of_elems;
    
    std::cin >> number_of_elems;
    std::vector<int> numbers(number_of_elems, 0);
    for (size_t i = 0; i < number_of_elems; ++i) {
        std::cin >> numbers[i];
    }
    
    SeqVector alternating_end_greater(number_of_elems, {1, kNoSequence});
    SeqVector alternating_end_less(number_of_elems, {1, kNoSequence});
    
    CreateSubsequences(numbers, &alternating_end_greater, &alternating_end_less);
    RestoreAnswer(alternating_end_greater, alternating_end_less, numbers);
    return 0;
}
