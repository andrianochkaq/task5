#pragma once
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "Table.h"
using namespace std;

// Inverted index: слово -> відсортований posting list (номери записів)
class InvertedIndex {
    map<string, vector<int>> index_;
public:
    void addRecord(int id, const vector<string>& words) {
        set<string> unique(words.begin(), words.end());   // слово раз на запис
        for (const auto& w : unique) index_[w].push_back(id);
    }

    static string listToString(const vector<int>& list) {
        string s;
        for (size_t i = 0; i < list.size(); ++i) s += to_string(list[i]) + (i + 1 < list.size() ? ", " : "");
        return s.empty() ? "-" : s;
    }

    void print() const {
        Table t({"Слово", "Записів", "Posting list"});
        for (const auto& item : index_) {
            const string& word = item.first;          // слово
            const vector<int>& list = item.second;    // його posting list
            t.add({word, to_string(list.size()), listToString(list)});
        }
        t.print();
        cout << "Різних слів в індексі: " << index_.size() << "\n";
    }

    const vector<int>& postings(const string& w) const {
        static const vector<int> empty;
        auto it = index_.find(w);
        return it == index_.end() ? empty : it->second;
    }

    // Завдання Б: кількість записів зі словом w
    size_t countRecords(const string& w) const { return postings(w).size(); }

    // Завдання В: перетин трьох posting list
    vector<int> intersect3(const string& a, const string& b, const string& c) const {
        vector<int> tmp, res;
        set_intersection(postings(a).begin(), postings(a).end(),
                         postings(b).begin(), postings(b).end(), back_inserter(tmp));
        set_intersection(tmp.begin(), tmp.end(),
                         postings(c).begin(), postings(c).end(), back_inserter(res));
        return res;
    }
};