#include <iostream>
#include <vector>
using namespace std;

const int MAXN = 2e6, MAXQ = 2e5;
int size[MAXN], leaf[MAXN], parent[MAXN], depth[MAXN];
int index[MAXN], tobegin[MAXN], marked[MAXN];
int paths[2*MAXN];
vector<int> children[MAXN];
vector<int> leavesatdepth[MAXN];

void dfs(int u) {
    size[u] = 1;
    leaf[u] = u;
    for (int v : children[u]) {
        parent[v] = u;
        depth[v] = depth[u]+1;
        dfs(v);
        leaf[u] = leaf[v];
        size[u] += size[v];
    }
}

int n, r=0;
int path_ind = 0;

void precompute() {
    parent[r] = r; // denotes the root
    depth[r] = 0;
    dfs(r);
    for (int u=0; u<n; ++u) {
        if (leaf[u] == u) {
            leavesatdepth[depth[u]].push_back(u);
        }
    }
    fill(marked, marked+n, false);
    for (int d=n-1; d>=0; --d) 
        for (int l : leavesatdepth[d]) {
            int v = l, w = l; // w is the vertex below v
            int max_path_len = 0;
            do {
                marked[v] = true;
                ++max_path_len;
                w = v;
                v = parent[v];
            // we don't go in when the vertex below v is the root of tree or when v is marked
            } while (depth[w] != 0 && !marked[v]);

            //Now we write the vertices from the doubled path in the paths array 
            //and number of edges to the begin of the corresponding path
            int double_path_len = max_path_len * 2 - 1;
            v = l;
            for (int ind = double_path_len-1; ind>=double_path_len-max_path_len; --ind) {
                paths[path_ind + ind] = v;
                index[v] = path_ind + ind;
                tobegin[v] = ind;
                v = parent[v];
            }
            for (int ind = double_path_len-max_path_len-1; ind>=0; --ind) {
                paths[path_ind + ind] = v;
                v = parent[v];
            }
            path_ind += double_path_len;
        }
}

int query(int u, int d) {
    if (u == r) {
        return r;
    }
    if (d <= tobegin[u]) {
        return paths[index[u] - d];
    }
    int v = parent[paths[index[u] - tobegin[u]]];
    int k = d - tobegin[u] - 1;
    return query(v, k);
}

int main() {
    cin >> n;

    for (int i=0; i<n-1; ++i) {
        int u, v;
        cin >> u >> v;
        children[u].push_back(v);
    }

    precompute();

    int q;
    cin >> q;

    while (q--) {
        int u, d;
        cin >> u >> d;
        //cout << "at " << q << endl;
        cout << query(u, d) << endl;
    }

    return 0;
}
