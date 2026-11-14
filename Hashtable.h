#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "Table.h"
using namespace std;

// Hash table з відкритою адресацією, m = 32.
// h1 - multiplication method, пробування - double hashing.
class HashTable {
public:
enum class State { EMPTY, USED, DELETED };
struct Cell {
State state = State::EMPTY;
string key;
int id = -1;
int h1 = -1, h2 = -1, probes = 0;   // для красивого виводу
};
struct RemoveResult { bool found = false; int pos = -1; int probes = 0; };

explicit HashTable(int m = 32) : m_(m), cells_(m) {}

bool insert(const string& key, int id) {
    uint64_t k = toNumber(key);
    int firstFree = -1, firstFreeProbe = 0;
    for (int i = 0; i < m_; ++i) {
        int pos = probe(k, i);
        if (cells_[pos].state == State::USED && cells_[pos].key == key) return false; // дубль
        if (cells_[pos].state != State::USED && firstFree < 0) { firstFree = pos; firstFreeProbe = i + 1; }
        if (cells_[pos].state == State::EMPTY) break;
    }
    if (firstFree < 0) return false;                 // таблиця повна
    cells_[firstFree] = {State::USED, key, id, h1(k), h2(k), firstFreeProbe};
    ++used_;
    if (firstFreeProbe > 1) collisions_ += firstFreeProbe - 1;
    return true;
}

// Завдання А: вилучення. Ставимо мітку DELETED (щоб не зламати ланцюжки проб)
RemoveResult remove(const string& key) {
    uint64_t k = toNumber(key);
    for (int i = 0; i < m_; ++i) {
        int pos = probe(k, i);
        if (cells_[pos].state == State::EMPTY) return {false, -1, i + 1};
        if (cells_[pos].state == State::USED && cells_[pos].key == key) {
            cells_[pos].state = State::DELETED;      // ключ лишаємо лише для виводу
            --used_;
            return {true, pos, i + 1};
        }
    }
    return {};
}

// highlight - номер комірки, яку треба позначити в примітці
void print(int highlight = -1) const {
    Table t({"Комірка", "Стан", "Ключ", "Запис №", "h1", "h2", "Проб", "Примітка"});
    for (int i = 0; i < m_; ++i) {
        const Cell& c = cells_[i];
        string note = (i == highlight) ? "<-- щойно вилучено" : "";
        if (c.state == State::EMPTY)
            t.add({to_string(i), "вільна", "-", "-", "-", "-", "-", note});
        else
            t.add({to_string(i), c.state == State::USED ? "зайнята" : "ВИЛУЧЕНА",
                   c.key, c.state == State::USED ? to_string(c.id) : "-",
                   to_string(c.h1), to_string(c.h2), to_string(c.probes), note});
    }
    t.print();
    cout << "Зайнято: " << used_ << " з " << m_ << "   коефіцієнт заповнення = "
         << static_cast<double>(used_) / m_ << "   зайвих проб (колізій) при вставці: "
         << collisions_ << "\n";
}
private:
int m_;
vector<Cell> cells_;
int used_ = 0, collisions_ = 0;

// рядок -> число (поліноміальна згортка)
static uint64_t toNumber(const string& s) {
    uint64_t k = 0;
    for (char c : s) k = k * 31 + static_cast<uint64_t>(c - 'a' + 1);
    return k;
}
// multiplication method: h1 = floor(m * frac(k * A)), A = (sqrt5 - 1)/2
int h1(uint64_t k) const {
    const double A = 0.6180339887498949;
    double x = fmod(static_cast<double>(k) * A, 1.0);
    return static_cast<int>(m_ * x);
}
// другий хеш має бути непарним (взаємно простим з m = 32)
int h2(uint64_t k) const { return 2 * static_cast<int>(k % (m_ / 2)) + 1; }

// double hashing: (h1 + i*h2) mod m
int probe(uint64_t k, int i) const { return (h1(k) + i * h2(k)) % m_; }
};