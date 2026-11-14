#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Довжина рядка в СИМВОЛАХ (а не в байтах): кирилиця в UTF-8 займає 2 байти,
// тому рахуємо лише байти, що не є "продовженням" символу (10xxxxxx).
inline size_t dispLen(const string& s) {
    size_t n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++n;
    return n;
}

// Дописує пробіли праворуч до ширини w
inline string pad(const string& s, size_t w) {
    size_t l = dispLen(s);
    return s + string(w > l ? w - l : 0, ' ');
}

// Заголовок розділу в рамці
inline void banner(const string& text) {
    string line(70, '=');
    cout << "\n" << line << "\n  " << text << "\n" << line << "\n";
}

// Проста ASCII-таблиця з автоматичною шириною стовпців
class Table {
    vector<string> headers_;
    vector<vector<string>> rows_;
public:
    explicit Table(vector<string> headers) : headers_(move(headers)) {}
    void add(vector<string> row) { rows_.push_back(move(row)); }

    void print() const {
        size_t cols = headers_.size();
        vector<size_t> w(cols);
        for (size_t c = 0; c < cols; ++c) w[c] = dispLen(headers_[c]);
        for (const auto& r : rows_)
            for (size_t c = 0; c < cols && c < r.size(); ++c) w[c] = max(w[c], dispLen(r[c]));

        auto sep = [&]() {
            cout << "+";
            for (size_t c = 0; c < cols; ++c) cout << string(w[c] + 2, '-') << "+";
            cout << "\n";
        };
        auto row = [&](const vector<string>& r) {
            cout << "|";
            for (size_t c = 0; c < cols; ++c)
                cout << " " << pad(c < r.size() ? r[c] : "", w[c]) << " |";
            cout << "\n";
        };
        sep(); row(headers_); sep();
        for (const auto& r : rows_) row(r);
        sep();
    }
};