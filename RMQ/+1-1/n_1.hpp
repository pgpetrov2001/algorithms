#ifndef N_1_1
#define N_1_1

#include <utility>
#include <cassert>
#include <cmath>
#include <optional>

#include "../nlogn_1.hpp"

using std::swap;
using ::log2;
using std::optional;

template<typename T, size_t MAXN>
class RMQ_1 {
    private:
        static constexpr int B_from_N(int N) {
            return log2(N) / 2;
        }

        static const int MAXB = B_from_N(MAXN);
        static const int MAXCODES = 1<<(MAXB-1);

        int n, B;
        const T (&arr)[MAXN];
        T blocks[MAXN/MAXB];
        optional<const RMQ<T, MAXN/MAXB>> blocks_rmq; // because it doesn't have a default constructor
        int block_code[MAXN/MAXB];
        int tables[MAXCODES][MAXB][MAXB];

        void compute_table(int code, int b) {
            for (int i=0; i<B; ++i) {
                int ans = i;
                for (int j=i; j<B; ++j) {
                    if (arr[b*B + j] < arr[b*B + ans])
                        ans = j;
                    tables[code][i][j] = ans;
                }
            }
        }

    public:
        RMQ_1(const T (&arr)[MAXN], int n) : arr(arr), n(n) {
            B = B_from_N(n);
            int code;
            for (int i=0; i<n; ++i) {
                if (i%B == 0) {
                    blocks[i/B] = arr[i];
                    code = 0;
                } else {
                    if (arr[i] < blocks[i/B])
                        blocks[i/B] = arr[i];

                    int diff = arr[i] - arr[i-1];
                    if (diff == -1) { // means 0
                        code <<= 1;
                    } else { // means 1
                        ++code;
                        code <<= 1;
                    }
                    if ((i+1)%B == 0) { // at last element in block
                        block_code[i/B] = code;
                        compute_table(code, i/B);
                    }
                }
            }
            blocks_rmq.emplace(blocks, n/B);
        }

        int query(int l, int r) const {
            if (l > r) swap(l, r);
            if (l/B == r/B) 
                return (l/B)*B + tables[block_code[l/B]][l%B][r%B];
            int lans = (l/B)*B + tables[block_code[l/B]][l%B][B-1];
            int rans = (r/B)*B + tables[block_code[r/B]][0][r%B]; 
            int ans;
            if (arr[lans] < arr[rans])
                ans = lans;
            else
                ans = rans;
            if (r/B - l/B > 1) {
                int bb = blocks_rmq->query(l/B+1, r/B-1);
                if (blocks[bb] < arr[ans]) {
                    int bans = bb*B + tables[block_code[bb]][0][B-1];
                    ans = bans;
                }
            }
            return ans;
        }
};

#endif
