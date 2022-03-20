#include <vector>
#include <memory>
#include <optional>
#include <limits>
#include <utility>
#include <bitset>
#include <cstdint>

using std::vector;
using std::unique_ptr;
using std::bitset;
using std::optional;
using std::size_t;

template <typename S_int, unsigned M=8*sizeof(S_int)>
class vEBTree {
    static_assert(M <= 8*sizeof(S_int));
    static constexpr S_int U = (
            M == 8*sizeof(S_int)?
            std::numeric_limits<S_int>::max():
            (S_int(1) << M) - 1
    );
    static constexpr int NAIVE_TH = 12;
public:
    friend class unique_ptr<vEBTree>;
    vEBTree() : vEBTree(M) {
    }
    vEBTree(int m) : m(m), size(0), min(U), max(0), H(), elements{} {
            if (m < NAIVE_TH)
                return;
            auto sz = 1<<(m/2);
            L.reserve(sz);
            for (size_t i=0; i<sz; ++i) {
                L.emplace_back(m/2);
            }
    }
    bool insert(S_int x);
    bool remove(S_int x);
    optional<S_int> prev(S_int x) const;

private:

    bool naive_insert(S_int x);
    bool naive_remove(S_int x);
    optional<S_int> naive_prev(S_int x) const;

    int m;
    S_int size;
    S_int min, max;
    unique_ptr<vEBTree> H;
    vector<vEBTree> L; // array, 1<<m
    bitset<(1<<NAIVE_TH)> elements;
};

template <typename S_int, unsigned M>
bool vEBTree<S_int,M>::naive_insert(S_int x) {
    if (elements[x])
        return false;
    elements[x] = true;
    ++size;
    return true;
}

template <typename S_int, unsigned M>
bool vEBTree<S_int,M>::naive_remove(S_int x) {
    if (!elements[x])
        return false;
    elements[x] = false;
    --size;
    return true;
}

template <typename S_int, unsigned M>
optional<S_int> vEBTree<S_int,M>::naive_prev(S_int x) const {
    for (int i=0; i<x; ++i)
        if (elements[i]) {
            return i;
        }
    return {};
}

template <typename S_int, unsigned M>
bool vEBTree<S_int,M>::insert(S_int x) {
    if (m <= NAIVE_TH) 
        return naive_insert(x);

    if (size == 0) {
        min = max = x;
        ++size;
        return true;
    }

    if (x == min)
        return false;
    if (x == max)
        return false;

    if (min == max) {
        if (x == min) // and max
            return false;
        min = std::min(min, x);
        max = std::max(max, x);
        ++size;
        return true;
    }

    if (x < min) 
        std::swap(x, min);
    if (x > max)
        std::swap(x, max);

    S_int factor = 1 << (m>>1);
    S_int hx = x >> (m>>1);
    S_int lx = x & (factor - 1); // equivalent to mod factor when factor is a power of 2

    if (!H)
        H = std::make_unique<vEBTree>(m/2);
        /* H = std::unique_ptr<vEBTree>(new vEBTree(m/2)); */

    if (L[hx].size == 0)
        H->insert(hx);

    if (L[hx].insert(lx)) {
        ++size;
        return true;
    }
    
    return false;
}

template <typename S_int, unsigned M>
bool vEBTree<S_int,M>::remove(S_int x) {
    if (size == 0)
        return false;
    if (x > max)
        return false;
    if (x < min)
        return false;
    if (m <= NAIVE_TH)
        return naive_remove(x);

    if (min == max) {
        min = U;
        max = 0;
        --size;
        return true;
    }

    if (size == 2) {
        if (x == min) {
            min = max;
            --size;
            return true;
        }
        if (x == max) {
            max = min;
            --size;
            return true;
        }
        return false;
    }

    S_int factor = 1 << (m>>1);

    if (x == min) {
        S_int hx_min = H->min;
        S_int lx_min = L[hx_min].min;
        x = min = hx_min * factor + lx_min;
    } else if (x == max) {
        S_int hx_max = H->max;
        S_int lx_max = L[hx_max].max;
        x = max = hx_max * factor + lx_max;
    }

    S_int hx = x >> (m>>1);
    S_int lx = x & (factor - 1); // equivalent to mod factor when factor is a power of 2

    if (!L[hx].remove(lx)) 
        return false;

    if (L[hx].size == 0)
        H->remove(hx);

    --size;
    return true;

}

template <typename S_int, unsigned M>
optional<S_int> vEBTree<S_int,M>::prev(S_int x) const {
    if (size == 0)
        return {};
    if (x >= max)
        return max;
    if (x < min)
        return {};
    if (m <= NAIVE_TH)
        return naive_prev(x);
    if (size == 2) 
        return min;

    S_int factor = 1 << (m>>1);
    S_int hx = x >> (m>>1);
    S_int lx = x & (factor - 1); // equivalent to mod factor when factor is a power of 2

    auto l_result = L[hx].prev(lx);
    if (l_result) {
        S_int prev_lx = *l_result;
        return hx * factor + prev_lx;
    }

    auto r_result = H->prev(hx-1);
    if (r_result) {
        S_int prev_hx = *r_result;
        S_int prev_lx = L[prev_hx].max;
        return prev_hx * factor + prev_lx;
    }

    return min;
}
