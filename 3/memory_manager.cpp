//
//  main.cpp
//  MemoryManager
//
//  Created by Александр Марков on 08/11/2017.
//  Copyright © 2017. All rights reserved.
//
#include <iostream>
#include <vector>
#include <memory>


struct MemoryBlock {
    bool free;
    int left, right;
    size_t index;
    
    MemoryBlock* next, *prev;
    
    MemoryBlock(int left_ = 0, int right_ = 0, size_t index_ = 0,
                bool free_ = true,
                MemoryBlock* next_ =  nullptr,
                MemoryBlock* prev_ = nullptr)
    : free(free_), left(left_), right(right_),
    index(index_), next(next_), prev(prev_)
    {}
};

using pointer = MemoryBlock*;

class Heap {
public:
    Heap() {
        size_ = 0;
        heap_.push_back(nullptr);
    }
    
    ~Heap() {
        for (auto& elem : heap_) {
            delete elem;
        }
    }
    
    bool IsEmpty() const {
        return size_ == 0;
    }
    
    pointer FindMax() const {
        return heap_[1];
    }
    
    void Add(pointer block) {
        block->index = ++size_;
        heap_.push_back(block);
        Up(size_);
    }
    
    void Remove(size_t index) {
        my_swap(size_--, index);
        heap_.pop_back();
        Up(index);
        Down(index);
    }
    
    void Up(size_t child) {
        size_t parent = child / 2;
        if (parent == 0) return;
        if (compare(parent, child)) {
            my_swap(parent, child);
            Up(parent);
        }
    }
    
    void Down(size_t parent) {
        size_t child = 2 * parent;
        if (child > size_)
            return;
        if (child + 1 <= size_ && compare(child, child + 1)) {
            child++;
        }
        if (compare(parent, child)) {
            my_swap(parent, child);
            Down(child);
        }
    }

private:
    size_t size_;
    std::vector<pointer> heap_;
    
    void my_swap(size_t ii, size_t ij) {
        pointer block = heap_[ii];
        heap_[ii] = heap_[ij];
        heap_[ij] = block;
        heap_[ij]->index = ij;
        heap_[ii]->index = ii;
    }
    
    bool compare(size_t lhs, size_t rhs) {
        if (heap_[lhs]->right - heap_[lhs]->left < heap_[rhs]->right - heap_[rhs]->left) {
            return true;
        }
        if (heap_[lhs]->right - heap_[lhs]->left > heap_[rhs]->right - heap_[rhs]->left) {
            return false;
        }
        return heap_[lhs]->left > heap_[rhs]->left;
    }
};

int allocate (Heap* heap, std::vector<pointer>* queries, int number, size_t req_n) {
    if (heap->IsEmpty()) {
        return -1;
    }
    pointer top = heap->FindMax();
    if (top->right - top->left < number) {
        return -1;
    }
    int ans = static_cast<int>(top->left);
    if (top->right - top->left == number) {
        top->free = 0;
        (*queries)[req_n] = top;
        heap->Remove(1);
    } else {
        pointer block_to_add = new MemoryBlock(top->left, top->left + number, 0, 0, top, top->prev);
        if (top->prev) {
            top->prev->next = block_to_add;
        }
        top->prev = block_to_add;
        top->left = top->left + number;
        (*queries)[req_n] = block_to_add;
        heap->Down(1);
    }
    return ans + 1;
}


void free (Heap* heap, std::vector<pointer>* queries, int req_n) {
    if ((*queries)[req_n] == nullptr) {
        return;
    }
    pointer b_remove = (*queries)[req_n];
    pointer prev = b_remove->prev;
    pointer next = b_remove->next;
    (*queries)[req_n] = nullptr;
    if (prev && prev->free && next && next->free) {
        prev->right = next->right;
        heap->Up(prev->index);
        heap->Remove(next->index);
        prev->next = next->next;
        if (next->next) {
            next->next->prev = prev;
        }
        delete b_remove;
        delete next;
        return;
    }
    if (prev && prev->free) {
        prev->right = b_remove->right;
        heap->Up(prev->index);
        prev->next = next;
        if (next) {
            next->prev = prev;
        }
        delete b_remove;
        return;
    }
    if (next && next->free) {
        next->left = b_remove->left;
        heap->Up(next->index);
        next->prev = prev;
        if (prev) {
            prev->next = next;
        }
        delete b_remove;
        return;
    }
    b_remove->free = 1;
    heap->Add(b_remove);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    Heap heap;
    size_t n_cells, n_queries;
    std::cin >> n_cells >> n_queries;
    std::vector<pointer> queries (n_queries + 1, nullptr);
    pointer block = new MemoryBlock(0, static_cast<int>(n_cells), 1, 1);
    heap.Add(block);
    std::vector<int> results;
    for (size_t query = 1; query <= n_queries; ++query) {
        int ask;
        std::cin >> ask;
        if (ask > 0) {
            results.push_back(allocate(&heap, &queries, ask, query));
        } else {
            free(&heap, &queries, -ask);
        }
    }
    for (int result: results) {
        std::cout << result << "\n";
    }
    for (auto& query: queries) {
        delete query;
    }
    return 0;
}
