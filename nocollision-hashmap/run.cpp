#include <iostream>
#include <vector>
#include <cstdint>

#include "HashSet.hpp"

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    vector<uint64_t> S;
    S.resize(n);

    const unsigned int U = (1ll<<32)-5;
    //allowed values are 0 to U-1

    for (int i=0; i<n; ++i) {
        cin >> S[i];
        assert(S[i] < U);
    }

    auto set = new HashSet<uint64_t, U>(S);

    int q;
    cin >> q;

    while (q--) {
        uint64_t val;
        cin >> val;
        cout << set->query(val) << endl;
    }

    return 0;
}
