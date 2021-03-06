#include <cassert>
#include <utility>
#include <vector>
#include <random>
#include <cstdint>
#include <optional>

#ifndef HASHMAP
#define HASHMAP

using std::vector;
using std::pair;

template<typename S_int, typename Val, S_int U>
class HashMap {
    private:
        //the integral type S_int must have
        //the capacity to contain the number (U-1)*(U-1)
        //which might need to be calculated in the hash function,
        //which will lead to an error in the calculations and might cause a hash collision
        static constexpr S_int next_pow2_bit(S_int x, S_int pow=1, int bit=0) {
            return x>pow? next_pow2_bit(x, pow*2, bit+1): bit;
        }
        static_assert(
            std::is_integral<S_int>::value && std::is_unsigned<S_int>::value,
            "Unsigned integral value required for underlying type of the set S"
        );
        static_assert(
            next_pow2_bit(U) <= sizeof(S_int) * 4,
            "Too large U, cannot gurantee no overflow with size of integral type"
        );

        static constexpr bool is_composite(S_int x, S_int d1=2, S_int d2=U-1) {
            return d1*d1>x? false: d1==d2? x%d1==0: is_composite(x, d1, (d1+d2)/2) || is_composite(x, (d1+d2)/2+1, d2);
        }
        static constexpr S_int prime_gte(S_int x) { //has to choose p so that S_int^2 doesn't overflow
            return is_composite(x)? prime_gte(x+1): x;
        }
        static constexpr S_int p = prime_gte(U);

        //random generator
        std::mt19937 generator;
        std::uniform_int_distribution<S_int> distribution;
        const int n, m;
        vector<int> dataindi; // At which index is the hash array for box with hash i
        S_int c;
        vector<S_int> ci, key_data;
        vector<Val> val_data;
        vector<vector<S_int>> Si; // Si = Hits(S, c, m, i)
        vector<vector<Val>> Vi; 

        int h(S_int c, int m, S_int x) {
            return ((c*x)%p)%m;
        }

        S_int col(const vector<S_int>& Si, S_int c, int m) { //Col(S, m, c)
            int ni = Si.size();
            vector<int> hits(m, 0);
            S_int ans = 0;
            for (S_int x : Si) {
                ans += hits[h(c, m, x)];
                hits[h(c, m, x)]++;
            }
            return ans;
        }

    public:
        HashMap(const vector<pair<S_int,Val>> &keyval) : 
            n(keyval.size()), Si(keyval.size()), Vi(keyval.size()), 
            m(keyval.size()), ci(keyval.size()), dataindi(keyval.size()), distribution(1, p-1) { 
            //no gurantees for correctness when S contains an element >= U
            if (keyval.empty()) {
                return;
            }
            vector<S_int> S;
            S.reserve(keyval.size());
            for (auto &[key, val] : keyval) {
                S.push_back(key);
            }

            S_int guess_c;
            do {
                guess_c = distribution(generator);
            } while (col(S, guess_c, m) > ((n*(n-1))/2)/m);
            c = guess_c;

            for (auto &[key, val] : keyval) {
                int hash = h(c, m, key);
                Si[hash].push_back(key);
                Vi[hash].push_back(val);
            }

            for (int i=0; i<m; ++i) {
                int ni = Si[i].size();
                if (ni == 0) continue;
                do {
                    guess_c = distribution(generator);
                } while (col(Si[i], guess_c, ni*ni) > 0);
                ci[i] = guess_c;

                dataindi[i] = key_data.size();
                key_data.resize(key_data.size() + ni*ni, U);
                val_data.resize(val_data.size() + ni*ni);
                for (int j=0; j<ni; ++j) {
                    auto key  = Si[i][j];
                    auto &val = Vi[i][j];
                    int idx = dataindi[i] + h(ci[i], ni*ni, key);
                    key_data[idx] = key;
                    val_data[idx] = val;
                }
            }
        }

        std::optional<Val> query(S_int key) {
            if (Si.empty()) {
                return {};
            }
            int i = h(c, m, key);
            if (Si[i].empty()) {
                return {};
            }
            int ni = Si[i].size();
            int idx = dataindi[i] + h(ci[i], ni*ni, key);
            if (key_data[idx] == key) {
                return val_data[idx];
            }
            return {};
        }
};

#endif
