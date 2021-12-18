#include <iostream>
#include <vector>
#include <cstdint>

#include "HashMap.hpp"

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    vector<uint64_t> S, V;
    S.resize(n);
    V.resize(n);

    const unsigned int U = (1ll<<32)-5;
    //allowed values are 0 to U-1

    for (int i=0; i<n; ++i) {
        cin >> S[i];
        assert(S[i] < U);
    }
    for (int i=0; i<n; ++i) {
        cin >> V[i];
    }

    vector<pair<uint64_t, uint64_t>> arg;
    for (int i=0; i<n; ++i) {
        arg.push_back({S[i], V[i]});
    }

    auto map = new HashMap<uint64_t, uint64_t, U>(arg);

    int q;
    cin >> q;

    while (q--) {
        uint64_t key;
        cin >> key;
        auto [exists, val] = map->query(key);
        if (exists) {
            cout << val << endl;
        } else {
            cout << "undefined" << endl;
        }
    }

    return 0;
}
