#include <iostream>
#include <vector>

#include "willard_algorithm.hpp"

using std::cin;
using std::cout;
using std::endl;

int main() {
    const uint64_t U = (1ll<<32)-5;
    std::vector<uint64_t> S;
    int n;
    cin >> n;
    S.resize(n);

    for (int i=0; i<n; ++i) {
        cin >> S[i];
    }

    FusionTree<uint64_t, U> tree(S);

    int q;
    cin >> q;

    while (q--) {
        S_int x;
        cin >> x;
        auto result = tree.query(x);
        if (result) {
            cout << *result;
        } else {
            cout << "undefined";
        }
        cout << endl;
    }
}
