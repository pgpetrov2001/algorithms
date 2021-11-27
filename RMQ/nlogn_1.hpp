#ifndef NLOGN_1
#define NLOGN_1

#include <utility>
using std::swap;
#include <cassert>
#include <cmath>
using ::log2;

template<typename T, size_t MAXN>
class RMQ {
    private:
        static const int MAXLOGN = log2(MAXN);
        int n;
        const T (&arr)[MAXN];
        int jump_left[MAXN][MAXLOGN];
        int jump_right[MAXN][MAXLOGN];
        int log_table[MAXN];
        int pow_table[MAXLOGN];

    public:
        int better(int ind1, int ind2) {
            return arr[ind1] < arr[ind2]? ind1: ind2;
        }

        RMQ(const T (&arr)[MAXN], int n) : arr(arr), n(n) {
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
            for (int i=0; i<n; ++i) {
                jump_left[i][0] = i;
                int j = i-1;
                int log = 0;
                while (j >= 0) {
                    jump_left[i][log+1] = better(jump_left[j][log], jump_left[i][log]);
                    log++;
                    j += j-i;
                }
                while (log <= maxlog-1) {
                    jump_left[i][log+1] = jump_left[i][log];
                    log++;
                }
            }
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

        int query(int l, int r) {
            if (l > r) swap(l, r);
            if (l == r) return l;
            int amt = log_table[r-l];
            assert(2*(1<<amt) >= r-l+1);
            return better(jump_right[l][amt], jump_left[r][amt]);
        }
};

#endif
