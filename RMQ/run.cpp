#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include "nlogn_1.hpp"
#include "+1-1/n_1.hpp"
#include "n_1.hpp"

const size_t MAXN = 1e6;
int arr[MAXN];

int main() {
    size_t n;
    cin >> n;

    for (size_t i=0; i<n; ++i) {
        cin >> arr[i];
    }

    auto rmq = new RMQ_const(arr, n);

    int q;
    cin >> q;

    while (q--) {
        size_t l, r;
        cin >> l >> r;
        cout << arr[rmq->query(l, r)] << endl;
    }
}
