#ifndef N_1_1
#define N_1_1

#include <optional>

#include "../nlogn_1.hpp"

using std::swap;
using ::log2;
using ::ceil;
using std::optional;
using std::move;
using std::array;

template<typename T>
class RMQ_1 {
    private:
        static constexpr int B_from_LOGN(int LOGN) {
            return LOGN / 2;
        }
        static constexpr int B_from_N(int N) {
            return B_from_LOGN(log2(N));
        }

        static const int MAXB = B_from_LOGN(32);

        int B;
        const vector<T> arr;
        vector<T> blocks;
        optional<const RMQ<T>> blocks_rmq; // because it doesn't have a default constructor
        vector<int> block_code;
        vector< array<array<int, MAXB>, MAXB> > tables;

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
        RMQ_1(const vector<T> arr) : arr(move(arr)) {
            int n = this->arr.size();
            B = B_from_N(n);
            blocks.resize(n / B);
            block_code.resize(n / B);
            tables.resize(1<<(B-1));
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
                        code <<= 1;
                        ++code;
                    }
                    if ((i+1)%B == 0) { // at last element in block
                        block_code[i/B] = code;
                        compute_table(code, i/B);
                    }
                }
            }
            blocks_rmq.emplace(blocks);
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
