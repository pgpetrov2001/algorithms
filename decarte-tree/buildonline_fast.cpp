#include <iostream>
#include <cassert>
using namespace std;

const int MAXN = 2e6;
int arr[MAXN];
int parent[MAXN];

int main() {
    int n;
    std::cin >> n;

    for (int i=0; i<n; ++i) {
        std::cin >> arr[i];
    }

    parent[0] = -1;

    for (int i=1; i<n; ++i) {
        int v = i-1;
        int c = -1;
        while (v != -1 && arr[i] < arr[v]) {
            c = v;
            v = parent[v];
        }
        if (c != -1) {
            parent[c] = i;
        }
        parent[i] = v;
    }

    for (int i=0; i<n; ++i) {
        cout << arr[i] << ": " << arr[parent[i]] << endl;
    }
}
//10
//-7 6 4 1 17 2 25 42 3 8

