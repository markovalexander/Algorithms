//
//  main.cpp
//  FixedSet
//
//  Created by Александр Марков on 31/10/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <vector>
#include <random>

constexpr int64_t kPrimeModulo = 2000000000 + 11;
constexpr int64_t kDispersion = 4;

class HashFunction;

template<typename T>
class BucketElem;

class Bucket;
class FixedSet;

std::vector<int64_t> ReadVector();

std::vector<int> EmulateQueries(const std::vector<int64_t>& numbers,
                                const std::vector<int64_t>& queries);
void PrintOutput(const std::vector<int>& results);

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    std::vector<int64_t> numbers = ReadVector();
    std::vector<int64_t> queries = ReadVector();
    
    std::vector<int> results = EmulateQueries(numbers, queries);
    PrintOutput(results);
    return 0;
}


class HashFunction {
public:
    HashFunction() {}
    
    explicit HashFunction(size_t bucket_size, std::default_random_engine& generator)
    : bucket_size_(bucket_size)
    {
        seed_ = std::uniform_int_distribution<int64_t>(1, kPrimeModulo - 1)(generator);
        shift_ = std::uniform_int_distribution<int64_t>(0, kPrimeModulo - 1)(generator);
    }
    
    const int64_t operator()(int64_t value) const {
        int64_t positive_number = value + kPrimeModulo;
        return (seed_ * positive_number % kPrimeModulo + shift_) % kPrimeModulo % bucket_size_;
    }
    
    const size_t GetSize() const {
        return bucket_size_;
    }
    
private:
    int64_t seed_;
    int64_t shift_;
    size_t bucket_size_;
};

template<typename T>
class BucketElem {
public:
    BucketElem()
    : has_value_(false)
    {}
    
    explicit BucketElem(const T& value)
    : value_(value), has_value_(true)
    {}
    
    BucketElem(const T& value, bool is_value_in)
    : value_(value), has_value_(is_value_in)
    {}
    
    void SetValue(const T& value) {
        value_ = value;
        has_value_ = true;
    }
    
    const T& GetValue() const {
        return value_;
    }
    
    const bool HasValue() const {
        return has_value_;
    }
    
private:
    T value_;
    bool has_value_;
};

class Bucket {
public:
    Bucket() {}
    
    void Initialize(const std::vector<int64_t>& elements,
                    std::default_random_engine& generator) {
        hash_ = GenerateHash(elements, generator);
        hash_table_.resize(elements.size() * elements.size());
        for (int64_t elem: elements) {
            int64_t elem_hash = hash_(elem);
            hash_table_[elem_hash].SetValue(elem);
        }
    }
    
    bool Contains(int64_t value) const {
        if (hash_table_.empty()) {
            return false;
        }
        int64_t hash = hash_(value);
        BucketElem<int64_t> checker = hash_table_[hash];
        if (checker.HasValue() && checker.GetValue() == value) {
            return true;
        }
        return false;
    }
    
private:
    HashFunction hash_;
    std::vector<BucketElem<int64_t>> hash_table_;
    
    bool CheckHashFunction(const std::vector<int64_t>& elements, const HashFunction& hash) {
        std::vector<size_t> bucket_sizes(elements.size() * elements.size(), 0);
        for (int64_t elem: elements) {
            int64_t elem_hash = hash(elem);
            ++bucket_sizes[elem_hash];
            if (bucket_sizes[elem_hash] > 1) {
                return false;
            }
        }
        return true;
    }
    
    HashFunction GenerateHash(const std::vector<int64_t> &elements,
                              std::default_random_engine &generator) {
        size_t bucket_size = elements.size() * elements.size();
        HashFunction hash_function;
        bool success_flag = false;
        do {
            hash_function = HashFunction{bucket_size, generator};
            success_flag = CheckHashFunction(elements, hash_function);
        } while (!success_flag);
        return hash_function;
    }
};

class FixedSet {
public:
    FixedSet() {};
    
    void Initialize(const std::vector<int64_t>& numbers) {
        std::vector<std::vector<int64_t>> pre_table(numbers.size());
        std::default_random_engine generator;
        hash_ = GenerateHash(numbers, generator);
        
        for (int64_t number: numbers) {
            pre_table[hash_(number)].push_back(number);
        }
        buckets_.resize(numbers.size());
        for (size_t i = 0; i < pre_table.size(); ++i) {
            buckets_[i].Initialize(pre_table[i], generator);
        }
    }
    
    bool Contains(int64_t value) const {
        int64_t hash = hash_(value);
        return buckets_[hash].Contains(value);
    }
    
private:
    std::vector<Bucket> buckets_;
    HashFunction hash_;
    
    bool CheckHashFunction(const std::vector<int64_t>& numbers, const HashFunction& hash) {
        std::vector<int> collisions(numbers.size(), 0);
        for (int64_t number: numbers) {
            ++collisions[hash(number)];
        }
        size_t tresh_hold = 0;
        for (int collision: collisions) {
            tresh_hold += static_cast<size_t>(std::pow(collision, 2));
        }
        return tresh_hold < kDispersion * numbers.size();
    }
    
    HashFunction GenerateHash(const std::vector<int64_t> &numbers,
                              std::default_random_engine& generator) {
        size_t bucket_size = numbers.size();
        HashFunction hash_function;
        bool success_flag = false;
        do {
            hash_function = HashFunction{bucket_size, generator};
            success_flag = CheckHashFunction(numbers, hash_function);
        } while (!success_flag);
        return hash_function;
    }
};

std::vector<int64_t> ReadVector() {
    size_t n_elems;
    std::cin >> n_elems;
    std::vector<int64_t> answer(n_elems);
    
    for (size_t i = 0; i < n_elems; ++i) {
        std::cin >> answer[i];
    }
    
    return answer;
}


std::vector<int> EmulateQueries(const std::vector<int64_t>& numbers,
                                const std::vector<int64_t>& queries) {
    FixedSet set;
    set.Initialize(numbers);
    std::vector<int> answers(queries.size(), 0);
    for (size_t i = 0; i < queries.size(); ++i) {
        answers[i] = (set.Contains(queries[i]) ? 1 : -1);
    }
    return answers;
}

void PrintOutput(const std::vector<int>& results) {
    for (int result: results) {
        std::cout << (result == 1 ? "Yes\n" : "No\n");
    }
}
