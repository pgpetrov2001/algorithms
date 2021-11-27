#include <iostream>
#include <vector>
using namespace std;

const int MAXN = 2e6, MAXQ = 2e5;
const int MAXLOGN = 21;
int logtable[MAXN], powtable[MAXLOGN];
int size[MAXN], leaf[MAXN], parent[MAXN], depth[MAXN];
int index[MAXN], tobegin[MAXN], marked[MAXN];
int paths[MAXN*2], jump[MAXN][MAXLOGN];
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

void precompute_pows_and_logs() {
    int log = 0, pow = 2;
    powtable[0] = 1;
    for (int num=1; num<n; ++num) {
        if (num >= pow) {
            ++log;
            powtable[log] = pow;
            pow *= 2;
        }
        logtable[num] = log;
    }
}

void precompute_path_from_leaf(int l) {
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

void precompute_jumps_from_leaf(int l) {
    int v = parent[l];
    jump[l][0] = v;
    int j = 1, d = 1;
    while (d*2 < n) { // n-1 is the maximum jump
        if (d <= tobegin[v])
            v = paths[index[v] - d];
        else 
            v = r;
        jump[l][j] = v;
        d *= 2;
        ++j;
    }
}

void precompute() {
    precompute_pows_and_logs();
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
            precompute_path_from_leaf(l);
            precompute_jumps_from_leaf(l);
        }
}

int query(int u, int d) {
    int l = leaf[u];
    d += depth[l] - depth[u];
    if (d == 0) {
        return u;
    }
    if (d > n-1) {
        return r;
    }
    int j = logtable[d];
    int v = jump[l][j];
    d -= powtable[j];
    return paths[index[v] - d];
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
        cout << query(u, d) << endl;
    }
}
