#ifndef N_1
#define N_1

#include "+1-1/n_1.hpp"

template<typename T, size_t MAXN>
class RMQ_const {
    private:
        static const int MAX_WALK = 2*MAXN-1;

        int n;
        const T (&arr)[MAXN];
        int parent[MAXN], left[MAXN], right[MAXN], root;
        int nodearr[MAX_WALK], deptharr[MAX_WALK], arridx = 0;
        optional<const RMQ_1<T, MAX_WALK>> rmq; // because it doesn't have default constructor
        int walk_idx[MAXN];

        void create_decarte_tree() {
            left[0] = right[0] = parent[0] = -1;

            for (int toadd=1; toadd<n; ++toadd) {
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
            walk_idx[node] = arridx;
            nodearr[arridx] = node;
            deptharr[arridx] = depth;
            ++arridx;
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
        RMQ_const(const T (&arr)[MAXN], int n) : arr(arr), n(n) {
            create_decarte_tree();
            euler_walk_tree(root);
            rmq.emplace(deptharr, 2*n-1);
        }

        int query(int l, int r) const {
            return nodearr[rmq->query(walk_idx[l], walk_idx[r])];
        }
};

#endif
