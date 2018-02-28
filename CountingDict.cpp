    //
    //  main.cpp
    //  kThStatiscs
    //
    //  Created by Александр Марков on 08/11/2017.
    //  Copyright © 2017. All rights reserved.
    //


#include <vector>
#include <iostream>

class CountingDict {
    static constexpr int64_t kBits = 7;
    static constexpr int64_t kSeed = 377459;
    static constexpr int64_t kMod = kBits * 1210011;
    static constexpr int64_t kPrimeModulo = 1000000000 + 9;
    static constexpr int64_t kBucketSize = kMod / kBits;

public:
    CountingDict() {
        buckets_.assign(kMod, false);
    }
    
    void Insert(int64_t number) {
        int64_t hash = GetHash(number);
        int64_t counter = CalcNumber(hash * kBits + 1, hash * kBits + kBits + 1);
        SetNumber(counter + 1, hash * kBits + 1, hash * kBits + kBits + 1);
    }
    
    int GetCount(int64_t number) const {
        int64_t hash = GetHash(number);
        return static_cast<int> (CalcNumber(hash * kBits + 1, hash * kBits + kBits + 1));
    }
    
private:
    std::vector<bool> buckets_;
    
    int64_t GetHash(int64_t number) const {
        int64_t hash = (number * kSeed) % kPrimeModulo % kBucketSize;
        hash += (hash < 0 ? - hash - hash + 1 : 0);
        return hash;
    }
    
    int64_t CalcNumber(int64_t start, int64_t finish) const {
        int64_t counter = 0;
        int64_t degree = 1;
        for (int64_t i = start; i < finish; ++i) {
            if (buckets_[i])
                counter += degree;
            degree *= 2;
        }
        return counter;
    }
    
    void SetNumber(int64_t value, int64_t start, int64_t finish) {
        for (int64_t i = start; i < finish; ++i) {
            buckets_[i] = (value % 2);
            value = value / 2;
        }
    }
};
