#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <windows.h>
#include "Generator.h"
#include "HashTable.h"
#include "Inverterdindex.h"
#include "Table.h"
using namespace std;

struct Record { int id; string key; vector<string> words; };

string joinWords(const vector<string>& w) {
string s;
for (size_t i = 0; i < w.size(); ++i) s += w[i] + (i + 1 < w.size() ? " " : "");
return s;
}

// Таблиця записів з переданого списку номерів
void printRecords(const vector<Record>& recs, const vector<int>& ids) {
Table t({"№", "Ключ", "Текст"});
for (int id : ids) t.add({to_string(id), recs[id - 1].key, joinWords(recs[id - 1].words)});
t.print();
}

// Перевірка генератора: середнє, дисперсія, хі-квадрат (10 інтервалів)
void testGenerator(MiddleProductGenerator& g, int n = 1000) {
vector<double> v(n);
double sum = 0;
for (auto& x : v) { x = g.next(); sum += x; }
double mean = sum / n, var = 0;
for (double x : v) var += (x - mean) * (x - mean);
var /= n;

const int K = 10;
vector<int> cnt(K, 0);
for (double x : v) cnt[static_cast<int>(x * K)]++;
double expected = static_cast<double>(n) / K, chi2 = 0;

Table t({"Інтервал", "Потрапило (O)", "Очікується (E)", "(O-E)^2/E"});
for (int i = 0; i < K; ++i) {
    double term = (cnt[i] - expected) * (cnt[i] - expected) / expected;
    chi2 += term;
    t.add({"[" + to_string(i / 10.0).substr(0, 3) + "; " + to_string((i + 1) / 10.0).substr(0, 3) + ")",
           to_string(cnt[i]), to_string(static_cast<int>(expected)), to_string(term).substr(0, 5)});
}
banner(" Перевірка генератора (" + to_string(n) + " чисел)");
t.print();
cout << "Середнє   = " << mean << "   (теоретично 0.5)\n";
cout << "Дисперсія = " << var << "   (теоретично 1/12 = " << 1.0 / 12 << ")\n";
cout << "chi2      = " << chi2 << "   (критичне при 9 ст. свободи, alpha = 0.05: 16.919)\n";
cout << "Висновок: " << (chi2 < 16.919 ? "chi2 менший за критичне - гіпотеза рівномірності приймається"
                                        : "chi2 більший за критичне - гіпотеза відхиляється") << "\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // щоб кирилиця коректно виводилась у консолі Windows
#endif
    MiddleProductGenerator gen(577215, 693147);

    banner("Генерація даних (middle-product, R0=577215, R1=693147)");

    // Попередньо задані слова, з яких складаються тексти
    const vector<string> vocab = {
        "apple",  "river",  "stone",  "cloud", "forest",
        "window", "garden", "bridge", "candle", "mirror",
        "winter", "silver", "thunder", "pencil", "harbor"
    };

    cout << "Набір із 15 слів, з яких складаються тексти:\n";
    Table tv({"1", "2", "3", "4", "5"});
    for (size_t i = 0; i < vocab.size(); i += 5)
        tv.add({vocab[i], vocab[i + 1], vocab[i + 2], vocab[i + 3], vocab[i + 4]});
    tv.print();

    vector<Record> records;
    set<string> usedKeys;
    for (int id = 1; id <= 20; ++id) {
        string key;
        do { key = gen.nextWord(); } while (usedKeys.count(key));   // ключі не повторюються
        usedKeys.insert(key);
        int n = gen.nextInt(3, 6);                                   // 3..6 слів у тексті
        vector<string> words;
        for (int i = 0; i < n; ++i)
            words.push_back(vocab[gen.nextInt(0, vocab.size() - 1)]); // слова лише з масиву vocab
        records.push_back({id, key, words});
    }
    cout << "\nЗгенеровані записи:\n";
    vector<int> all;
    for (const auto& r : records) all.push_back(r.id);
    printRecords(records, all);

    HashTable table(32);
    InvertedIndex invIndex;
    for (const auto& r : records) {
        table.insert(r.key, r.id);
        invIndex.addRecord(r.id, r.words);
    }
 lreo
    
banner(" Hash table (multiplication method + double hashing, m = 32)");
table.print();
banner("Inverted index (слово -> posting list)");
invIndex.print();

// ---------- ЗАВДАННЯ А, Б, В ----------
banner("ЗАВДАННЯ А. Вилучення запису за ключем");
string k;
cout << "Введіть ключ К: ";
cin >> k;
auto res = table.remove(k);
if (res.found) {
    cout << "Ключ \"" << k << "\" знайдено в комірці " << res.pos << " за " << res.probes
         << " пробу(и) і вилучено.Hash table після вилучення:\n";
    table.print(res.pos);
} else {
    cout << "Ключа \"" << k << "\" у таблиці немає (пошук завершився на вільній комірці за "
         << res.probes << " пробу(и)). Hash table без змін:\n";
    table.print();
}

    banner("ЗАВДАННЯ Б. Кількість записів, текст яких містить слово w");
    string w;
    cout << "Введіть слово w: ";
    cin >> w;
    const auto& pl = invIndex.postings(w);
    cout << "Posting list слова \"" << w << "\": [" << InvertedIndex::listToString(pl) << "]\n";
    cout << "ВІДПОВІДЬ: записів, що містять \"" << w << "\": " << pl.size() << "\n";
    if (!pl.empty()) printRecords(records, pl);

    banner("ЗАВДАННЯ В. Записи, текст яких містить усі три слова w1, w2, w3");
    string w1, w2, w3;
    cout << "Введіть три слова через пробіл: ";
    cin >> w1 >> w2 >> w3;
    cout << "Posting list \"" << w1 << "\": [" << InvertedIndex::listToString(invIndex.postings(w1)) << "]\n";
    cout << "Posting list \"" << w2 << "\": [" << InvertedIndex::listToString(invIndex.postings(w2)) << "]\n";
    cout << "Posting list \"" << w3 << "\": [" << InvertedIndex::listToString(invIndex.postings(w3)) << "]\n";
    auto both = invIndex.intersect3(w1, w2, w3);
    cout << "Перетин трьох списків: [" << InvertedIndex::listToString(both) << "]\n";
    if (both.empty()) cout << "ВІДПОВІДЬ: немає жодного запису, що містить усі три слова.\n";
    else { cout << "ВІДПОВІДЬ: знайдено записів: " << both.size() << "\n"; printRecords(records, both); }

    // ---------- ЗАВДАННЯ 4 ----------
    testGenerator(gen, 1000);
    return 0;
}