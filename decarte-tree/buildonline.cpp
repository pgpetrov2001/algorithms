#include <iostream>
#include <cassert>

const int MAXN = 2e6;
int arr[MAXN];
int left[MAXN], right[MAXN];
int root;

void traverse(int root) {
    if (root == -1) {
        return;
    }
    assert((left[root] == -1 || arr[left[root]] >= arr[root]));
    assert((right[root] == -1 || arr[right[root]] >= arr[root]));
    traverse(left[root]);
    std::cout << root << " ";
    traverse(right[root]);
}

int main() {
    int n;
    std::cin >> n;

    for (int i=0; i<n; ++i) {
        std::cin >> arr[i];
        right[i] = left[i] = -1;
    }

    root = 0;

    for (int i=1; i<n; ++i) {
        int curr_root = root;
        int parent = -1;
        while (right[curr_root] != -1 && arr[i] > arr[curr_root]) {
            parent = curr_root;
            curr_root = right[curr_root];
        }
        if (arr[i] > arr[curr_root]) {
            right[curr_root] = i;
        } else {
            left[i] = curr_root;
            if (curr_root == root) {
                root = i;
            } else {
                right[parent] = i;
            }
        }
    }
    traverse(root);
}
//10
//-7 6 4 1 17 2 25 42 3 8

