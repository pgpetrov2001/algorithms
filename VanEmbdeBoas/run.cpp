#include <iostream>

using std::cin;
using std::cout;
using std::endl;

#include "vEBTree.hpp"
int main() {
    vEBTree<int> data;

    int q;
    cin >> q;


    while (q--) {
        char c;
        cin >> c;

        if (c == 'a') {
            int x;
            cin >> x;
            cout << data.insert(x) << endl;
        } else if (c == 'r') {
            int x;
            cin >> x;
            cout << data.remove(x) << endl;
        } else if (c == 'p') {
            int x;
            cin >> x;
            auto res = data.prev(x);
            if (!res)
                cout << "undefined" << endl;
            else
                cout << *res << endl;
        }
    }
}
