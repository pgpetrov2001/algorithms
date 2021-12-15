#include <cassert>
#include <cstdint>
#include <vector>
#include <cmath>
#include <utility>
#include <optional>
using std::vector;

#include "../nocollision-hashmap/HashMap.hpp"

using S_int = std::uint64_t;

template <typename S_int, S_int U>
class FusionTree {
public:
    FusionTree(const vector<S_int> &S) {
        vector<int> trie[2] = {{0},{0}};
        vector<S_int> prefix_of = {0};
        //trie[bit][v] is vertex that branches with bit from v
        vector<int> trie_level_vertices[LOGU+1]; 

        for (auto x : S) {
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
                    max.push_back(0);
                    min.push_back(U);
                    prev.push_back(U);
                } 
                v = trie[bit][v];
                max[v] = std::max(max[v], x);
                min[v] = std::min(min[v], x);
                prefix_of[v] = prefix;
            }
        }

        for (auto x : S) {
            S_int curr_prev = U;
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

    std::optional<S_int> query(S_int x) {
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
            return x;
        }
        auto prefix      = (x >> (LOGU - notexists_level)) ^ 1;
        auto bit         = prefix & 1;
        auto [_, vertex] = trie_level[notexists_level].query(prefix);
        if (bit == 1) {
            auto [_, leaf] = trie_level[LOGU].query(min[vertex]);
            if (prev[leaf] == U) { // this means it has no previous element in the set
                return {};
            }
            return prev[leaf];
        } 
        return max[vertex];
    }

private:
    static const int LOGU = ::ceil(::log2(U)); //bits needed to represent
    using hash_map = HashMap<S_int, S_int, U>;
    vector<S_int> prev;
    vector<hash_map> trie_level = {{{{0, 0}}}}; // hash-set doesn't have default constructor
    vector<S_int> max = {0};
    vector<S_int> min = {0};
};
