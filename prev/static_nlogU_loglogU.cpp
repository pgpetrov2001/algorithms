#include <iostream>
#include <utility>
#include <cassert>
#include <cstdint>
#include <vector>
#include <cmath>
using std::vector;
using std::pair;
using std::cin;
using std::cout;
using std::endl;

#include "../nocollision-hashmap/HashMap.hpp"

using S_int = std::uint64_t;
const S_int U = (1ll<<32)-5;
using hash_map = HashMap<S_int, S_int, U>;
const int LOGU = ::ceil(::log2(U)); //bits needed to represent
const int MAXN = 4e6;

int n;
S_int S[MAXN];
S_int prev[MAXN];
/* S_int prev[MAXN]; */
//above is if you want to be able to answer next queries as well
vector<hash_map> trie_level = {{{{0, 0}}}}; // hash-set doesn't have default constructor
vector<S_int> max = {0};
vector<S_int> min = {0};

void precompute() {
    vector<int> trie[2] = {{0},{0}};
    vector<S_int> prefix_of = {0};
    //trie[bit][v] is vertex that branches with bit from v
    vector<int> trie_level_vertices[LOGU+1]; 

    for (int i=0; i<n; ++i) {
        auto x = S[i];
        int v = 0;
        S_int curr_prev = U;
        for (int level=1; level<=LOGU; ++level) {
            auto prefix = x >> (LOGU - level);
            auto bit = prefix & 1;

            if (!trie[bit][v]) { // trie doesn't have such a branch
                trie_level_vertices[level].push_back(trie[0].size());
                trie[bit][v] = trie_level_vertices[level].back();
                trie[0].push_back(0);
                trie[1].push_back(0);
                prefix_of.push_back(0);
                max.push_back(0);
                min.push_back(U);
            } 
            if (bit == 1 && trie[0][v]) {
                curr_prev = max[trie[0][v]];
            }
            v = trie[bit][v];
            max[v] = std::max(max[v], x);
            min[v] = std::min(min[v], x);
            prefix_of[v] = prefix;
        }
        prev[v] = curr_prev;
    }

    trie_level.reserve(LOGU+1);
    /* trie_level.push_back({{}}); */
    for (int level=1; level<=LOGU; ++level) {
        vector<pair<S_int,S_int>> trie_level_keyval;
        trie_level_keyval.reserve(trie_level_vertices[level].size());
        for (auto v : trie_level_vertices[level]) {
            /* cout << v << " " << bitset<23>(prefix_of[v]) << " " << bitset<23>(max[v]) << endl; */
            trie_level_keyval.push_back({prefix_of[v], v});
        }
        /* cout << "Creating hash level " << level << endl; */
        trie_level.push_back({trie_level_keyval});
    }
}

pair<bool,S_int> query(S_int x) {
    int exists_level = 0, notexists_level = LOGU+1;
    while (exists_level+1 < notexists_level) {
        int mid_level    = (exists_level + notexists_level) / 2;
        auto prefix      = x >> (LOGU - mid_level);
        auto [exists, _] = trie_level[mid_level].query(prefix);
        if (exists) {
            exists_level = mid_level;
        } else {
            notexists_level = mid_level;
        }
    }
    if (exists_level == LOGU) {
        return {true, x};
    }
    auto prefix      = (x >> (LOGU - notexists_level)) ^ 1;
    auto bit         = prefix & 1;
    auto [_, vertex] = trie_level[notexists_level].query(prefix);
    if (bit == 1) {
        auto [_, leaf] = trie_level[LOGU].query(min[vertex]);
        if (prev[leaf] == U) { // this means it has no previous element in the set
            return {false, S_int()};
        }
        return {true, prev[leaf]};
    } 
    return {true, max[vertex]};
}

int main() {
    cin >> n;
    //S.resize(n);

    for (int i=0; i<n; ++i) {
        cin >> S[i];
    }

    precompute();

    int q;
    cin >> q;

    while (q--) {
        S_int x;
        cin >> x;
        auto [defined, result] = query(x);
        if (defined) {
            cout << result;
        } else {
            cout << "undefined";
        }
        cout << endl;
    }
}
