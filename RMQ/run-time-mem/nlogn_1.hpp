#ifndef NLOGN_1
#define NLOGN_1

#include <utility>
#include <cassert>
#include <cmath>
#include <vector>
#include <array>

using std::swap;
using ::log2;
using std::vector;
using std::move;
using std::array;

template<typename T>
class RMQ {
    private:
        static const int MAXLOGN = 32;
        const vector<T> arr;
        vector<array<T, MAXLOGN>> jump_right;
        vector<int> log_table;
        int pow_table[MAXLOGN];

    public:
        int better(int ind1, int ind2) const {
            return arr[ind1] < arr[ind2]? ind1: ind2;
        }

        RMQ(const vector<T> arr) : arr(move(arr)) {
            int n = this->arr.size();
            log_table.resize(n);
            jump_right.resize(n);
            int log = 0;
            int nextpow = 2;
            for (int pow=1; pow<n; ++pow) {
                if (pow == nextpow) {
                    nextpow = pow*2;
                    log++;
                    pow_table[log] = pow;
                }
                log_table[pow] = log;
                assert((1<<log) <= pow && pow < (1<<(log+1)));
            }
            int maxlog = log;
            for (int i=n-1; i>=0; --i) {
                jump_right[i][0] = i;
                int j = i+1;
                int log = 0;
                while (j < n) {
                    jump_right[i][log+1] = better(jump_right[j][log], jump_right[i][log]);
                    log++;
                    j += j-i;
                }
                while (log <= maxlog-1) {
                    jump_right[i][log+1] = jump_right[i][log];
                    log++;
                }
            }
        }

        int query(int l, int r) const {
            if (l == r) return l;
            if (l > r) swap(l, r);
            int amt = log_table[r-l];
            assert(2*(1<<amt) >= r-l+1);
            return better(jump_right[l][amt], jump_right[r-(1<<amt)+1][amt]);
        }
};

#endif
