#include <algorithm>
#include <vector>
#include <utility>
#include <optional>

#include "XFastTrie.hpp"
#include "../nocollision-hashmap/HashMap.hpp"

using std::vector;
using std::pair;

#ifndef YFASTTRIE
#define YFASTTRIE

template <typename S_int, S_int U>
class YFastTrie {
public:
    YFastTrie(const vector<S_int> &S_) {
        int n = S_.size();

        vector<pair<S_int, int>> keyval;
        keyval.reserve(n);
        for (int i=0; i<n; ++i) {
            keyval.push_back({S_[i], i});
        }
        std::sort(keyval.begin(), keyval.end());

        index_of = new HashMap<S_int, int, U>(keyval);
        S.reserve(n);
        for (int i=0; i<n; ++i) {
            S.push_back(keyval[i].first); // we need S to be sorted
        }

        int k = n / BLOCKSZ;
        vector<S_int> S1;
        S1.reserve(k + 1);
        for (int i=0; i<=k; ++i) {
            S1.push_back(S[i * BLOCKSZ]);
        }
        block_trie = new XFastTrie<S_int, U>(S1);

    }

    std::optional<S_int> query(S_int x) {
        int low, high;
        if (S.size() < BLOCKSZ) {
            low = -1;
            high = S.size();
        } else {
            auto result = block_trie->query(x);
            if (!result.has_value()) {
                return {};
            }
            int i = *result;
            low   =   i   * BLOCKSZ;
            high  = (i+1) * BLOCKSZ;
        }
        high = std::min(high, int(S.size()));
        while (low + 1 < high) {
            int mid = (low + high) / 2;
            if (S[mid] <= x) {
                low = mid;
            } else {
                high = mid;
            }
        }
        if (low == -1) { 
            return {};
        }
        return *index_of->query(S[low]);
    }
private:
    static const int LOGU = ::ceil(::log2(U)); //bits needed to represent
    static const int BLOCKSZ = LOGU;
    vector<S_int> S;
    XFastTrie<S_int, U> *block_trie;
    HashMap<S_int, int, U> *index_of;
};

#endif
