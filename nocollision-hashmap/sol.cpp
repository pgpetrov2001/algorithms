#include <iostream>
#include <cassert>
#include <vector>
#include <random>
#include <cstdint>
using namespace std;

typedef uint64_t bigint;
//bigint p = 18446744073709551557llu; // 2^64-59
//bigint p = (1ll << 41) - 21ll;
bigint p = (1ll<<32)-5; 

//random generator
mt19937 gen;
uniform_int_distribution<bigint> distr(1, p-1);

int h(bigint c, int m, bigint x) {
    return ((c*x)%p)%m;
}

bigint col(const vector<bigint>& Si, bigint c, int m) { //Col(S, m, c)
    int ni = Si.size();
    vector<int> hits(m, 0);
    bigint ans = 0;
    for (bigint x : Si) {
        ans += hits[h(c, m, x)];
        hits[h(c, m, x)]++;
    }
    return ans;
}

int n;
bigint m, c;

vector<bigint> S;
vector<bigint> data;

const int MAXN = 1e7;
vector<bigint> Si[MAXN]; // Hits(S, c, m, i)
bigint ci[MAXN];
int dataindi[MAXN]; // At which index is the hash array for box with hash i

void precompute() {
    bigint guess_c;
    do {
        guess_c = distr(gen);
    } while (col(S, guess_c, m) > ((n*(n-1))/2)/m);
    c = guess_c;

    for (bigint x : S) {
        Si[h(c, m, x)].push_back(x);
    }

    for (int i=0; i<m; ++i) {
        int ni = Si[i].size();
        if (ni == 0) continue;
        do {
            guess_c = distr(gen);
        } while (col(Si[i], guess_c, ni*ni) > 0);
        ci[i] = guess_c;

        dataindi[i] = data.size();
        data.resize(data.size() + ni*ni, -1);
        for (bigint x : Si[i]) {
            data[dataindi[i] + h(ci[i], ni*ni, x)] = x;
        }
    }
}

bool query(bigint val) {
    int i = h(c,m,val);
    if (Si[i].empty()) {
        return false;
    }
    int ni = Si[i].size();
    int j = h(ci[i], ni*ni, val);
    return data[dataindi[i] + j] == val;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n;
    S.resize(n);

    for (int i=0; i<n; ++i) {
        cin >> S[i];
    }

    m = n;
    precompute();

    int q;
    cin >> q;

    for (int t=0; t<q; ++t) {
        bigint val;
        cin >> val;
        cout << query(val) << endl;
    }

    return 0;
}
