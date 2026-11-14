#pragma once
#include <cstdint>
#include <string>
using namespace std;

// Middle-product method: R(n+1) = середина добутку R(n-1) * R(n)
class MiddleProductGenerator {
    uint64_t prev_, cur_;
    static constexpr uint64_t MOD = 1000000;   // шестицифрові числа
public:
    MiddleProductGenerator(uint64_t r0, uint64_t r1) : prev_(r0), cur_(r1) {}

    // Повертає число в [0,1)
    double next() {
        uint64_t product = prev_ * cur_;             // до 12 цифр
        uint64_t middle = (product / 1000) % MOD;    // 6 середніх цифр
        prev_ = cur_;
        cur_ = middle;
        return static_cast<double>(cur_) / MOD;
    }
    // Ціле в [lo, hi]
    int nextInt(int lo, int hi) {
        return lo + static_cast<int>(next() * (hi - lo + 1));
    }
    // Слово з малих латинських літер довжиною 3..8
    string nextWord() {
        int len = nextInt(3, 8);
        string w;
        for (int i = 0; i < len; ++i) w += static_cast<char>('a' + nextInt(0, 25));
        return w;
    }
};