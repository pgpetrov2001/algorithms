#ifndef N_1
#define N_1

#include "+1-1/n_1.hpp"

template<typename T>
class RMQ_const {
    private:
        const vector<T> arr;
        int root;
        vector<int> parent, left, right;
        vector<int> nodearr, deptharr;
        optional<const RMQ_1<T>> rmq; // because it doesn't have default constructor
        vector<int> walk_idx;

        void create_decarte_tree(const vector<T> &arr) {
            left[0] = right[0] = parent[0] = -1;

            for (int toadd = 1; toadd < arr.size(); ++toadd) {
                int par = toadd-1;
                int child = -1;
                while (par != -1 && arr[toadd] < arr[par]) {
                    child = par;
                    par = parent[par];
                }
                if (child != -1) { // toadd has a child?
                    parent[child] = toadd;
                }
                left[toadd] = child;
                right[toadd] = -1;
                parent[toadd] = par;
                if (par == -1) {
                    root = toadd;
                } else {
                    right[par] = toadd;
                }
            }
        }

        void add_walk_element(int node, int depth) {
            walk_idx[node] = nodearr.size();
            nodearr.push_back(node);
            deptharr.push_back(depth);
        }

        void euler_walk_tree(int node, int depth=0) {
            if (node == -1) return;
            add_walk_element(node, depth);
            if (left[node] != -1) {
                euler_walk_tree(left[node], depth+1);
                add_walk_element(node, depth);
            }
            if (right[node] != -1) {
                euler_walk_tree(right[node], depth+1);
                add_walk_element(node, depth);
            }
        }
    public:
        RMQ_const(const vector<T> &arr) {
            int n = arr.size();
            walk_idx.resize(n);
            parent.resize(n);
            left.resize(n);
            right.resize(n);
            create_decarte_tree(arr);
            euler_walk_tree(root);

            parent = left = right = {};
            rmq.emplace(move(deptharr));
        }

        int query(int l, int r) const {
            return nodearr[rmq->query(walk_idx[l], walk_idx[r])];
        }
};

#endif
