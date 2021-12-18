#include <cassert>
#include <cstdint>
#include <vector>
#include <cmath>
#include <utility>
#include <optional>
using std::vector;

#include <iostream>
using std::cout; using std::endl;

#include "../nocollision-hashmap/HashMap.hpp"

//TODO: add copy constructors to all classes

template <typename S_int, S_int U>
class XFastTrie {
private:
    static const int LOGU = ::ceil(::log2(U)); //bits needed to represent
    using hash_map = HashMap<S_int, int, U>;
    vector<hash_map> trie_level = {{{{0, 0}}}}; // hash-set doesn't have default constructor
    vector<int> max = {0};
    vector<int> min = {0};
    vector<int> prev = {0};
    vector<S_int> S;

public:
    XFastTrie(const vector<S_int> &S) : S(S) {
        vector<int> trie[2] = {{0},{0}};
        vector<S_int> prefix_of = {0};
        //trie[bit][v] is vertex that branches with bit from v
        vector<int> trie_level_vertices[LOGU+1]; 

        for (int i=0; i<S.size(); ++i) {
            auto x = S[i];
            int v = 0;
            for (int level=1; level<=LOGU; ++level) {
                auto prefix = x >> (LOGU - level);
                auto bit = prefix & 1;

                if (!trie[bit][v]) { // trie doesn't have such a branch
                    trie_level_vertices[level].push_back(trie[0].size());
                    trie[bit][v] = trie_level_vertices[level].back();
                    trie[0].push_back(0);
                    trie[1].push_back(0);
                    prefix_of.push_back(0);
                    max.push_back(-1);
                    min.push_back(-1);
                    prev.push_back(-1);
                } 
                v = trie[bit][v];
                max[v] = max[v] == -1? i: S[max[v]] < x? i: max[v];
                min[v] = min[v] == -1? i: S[min[v]] > x? i: min[v];
                prefix_of[v] = prefix;
            }
        }

        for (auto x : S) {
            int curr_prev = -1;
            int v = 0;
            for (int level=1; level<=LOGU; ++level) {
                auto prefix = x >> (LOGU - level);
                auto bit = prefix & 1;
                if (bit == 1 && trie[0][v]) {
                    curr_prev = max[trie[0][v]];
                }
                v = trie[bit][v];
            }
            prev[v] = curr_prev;
        }

        trie_level.reserve(LOGU+1);
        for (int level=1; level<=LOGU; ++level) {
            vector<pair<S_int, int>> trie_level_keyval;
            trie_level_keyval.reserve(trie_level_vertices[level].size());
            for (auto v : trie_level_vertices[level]) {
                trie_level_keyval.push_back({prefix_of[v], v});
            }
            trie_level.push_back({trie_level_keyval});
        }
    }

    std::optional<S_int> query(S_int x) {
        int exists_level = 0, notexists_level = LOGU+1;
        while (exists_level+1 < notexists_level) {
            int mid_level = (exists_level + notexists_level) / 2;
            auto prefix   = x >> (LOGU - mid_level);
            auto result   = trie_level[mid_level].query(prefix);
            if (result.has_value()) {
                exists_level = mid_level;
            } else {
                notexists_level = mid_level;
            }
        }
        if (exists_level == LOGU) {
            return max[*trie_level[LOGU].query(x)];
        }
        auto prefix = (x >> (LOGU - notexists_level)) ^ 1;
        auto bit    = prefix & 1;
        int vertex  = *trie_level[notexists_level].query(prefix);
        if (bit == 1) {
            int leaf = *trie_level[LOGU].query(S[min[vertex]]);
            if (prev[leaf] == -1) { // this means it has no previous element in the set
                return {};
            }
            return prev[leaf];
        } 
        return max[vertex];
    }
};
