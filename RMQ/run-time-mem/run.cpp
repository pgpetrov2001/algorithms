#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include "nlogn_1.hpp"
#include "+1-1/n_1.hpp"
#include "n_1.hpp"

int main() {
    size_t n;
    cin >> n;

    vector<int> arr(n);

    for (size_t i=0; i<n; ++i) {
        cin >> arr[i];
    }

    RMQ_const rmq(arr);

    int q;
    cin >> q;

    while (q--) {
        size_t l, r;
        cin >> l >> r;
        cout << arr[rmq->query(l, r)] << endl;
    }
}
