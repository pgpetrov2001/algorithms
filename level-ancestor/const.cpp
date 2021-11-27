#include <iostream>
#include <vector>
using namespace std;

const int MAXN = 2e6, MAXQ = 2e5;
const int MAXLOGN = 21;
const int MAXB = MAXLOGN/4;
int B;

//basic stuff
int powtable[MAXLOGN], logtable[MAXN];
int size[MAXN], leaf[MAXN], parent[MAXN], depth[MAXN];
//path decomposition stuff and jump stuff
int index[MAXN], tobegin[MAXN], marked[MAXN];
int paths[MAXN], leafnum[MAXN], jump[MAXN/(MAXB+1)][MAXLOGN];
vector<int> children[MAXN];
vector<int> leavesatdepth[MAXN];
const int MAXISOMORPH = 1<<(2*(MAXB-1));
int root[MAXN], f1[MAXN], f[MAXN], code[MAXN];
int table[MAXB*MAXB*MAXISOMORPH];
int start[MAXISOMORPH];

int indf1 = 0;
int ind_table = 0;

void dfs_table(int u) {
    int sz = size[root[u]];
    int indu = ind_table + f[u]*sz;
    int indp = ind_table + f[parent[u]]*sz;
    table[indu] = f[u];
    for (int d=1; d<sz; ++d) {
        table[indu + d] = table[indp + d-1];
    } 
    for (int c : children[u]) {
        dfs_table(u);
    }
}

void compute_table(int r) {
    start[code[r]] = ind_table;
    for (int c : children[r]) {
        dfs_table(c);
    }
    ind_table += size[r]*size[r];
}

int num;
void dfs_isomorph(int u) { 
    f[u] = num;
    f1[indf1 + num] = u;
    ++num;
    for (int c : children[u]) {
        dfs_isomorph(u);
    }
}

void isomorph(int r) {
    num = 0;
    dfs_isomorph(r);
    f[r] = indf1;
    indf1 += size[r];
}

void dfs(int u) {
    size[u] = 1;
    leaf[u] = -1;
    for (int v : children[u]) {
        parent[v] = u;
        depth[v] = depth[u]+1;
        dfs(u);
        if (size[v] > B) {
            leaf[u] = leaf[v];
        }
        size[u] += size[v];
    }
    if (leaf[u] == -1) {
        leaf[u] = u;
    }
}

int euler_code(int u, int cd=0) {
    for (int v : children[u]) {
        cd <<= 1;
        cd = euler_code(v, cd);
        cd <<= 1;
        ++cd;
    }
    return cd;
}

int n, r=0;
int path_ind = 0;

void precompute_macro() {
    int log = 0, pow = 2;
    for (int num=1; num<n; ++num) {
        if (num >= pow) {
            ++log;
            powtable[log] = pow;
            pow *= 2;
        }
        logtable[num] = log;
    }
    parent[r] = r; // denotes the root
    depth[r] = 0;
    dfs(r);
    for (int u=0; u<n; ++u) {
        if (leaf[u] == u) {
            leavesatdepth[depth[u]].push_back(u);
        }
    }
    fill(marked, marked+n, false);
    int L = 0;
    for (int d=n-1; d>=0; --d) {
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
            int len = max_path_len * 2 - 1;
            v = l;
            for (int ind=len-1; ind>=0; --ind) {
                paths[path_ind + ind] = v;
                index[v] = path_ind + ind;
                tobegin[v] = ind;
                v = parent[v];
            }
            path_ind += len;

            leafnum[l] = L;
            //Now we calculate jump array, because all paths upwards are already computed
            v = parent[l];
            jump[L][0] = v;
            int j = 1, d = 1;
            while (d*2 < n) { // n-1 is the maximum jump
                if (d <= tobegin[v])
                    v = paths[index[v] - d];
                else
                    v = r;
                jump[L][j] = v;
                d *= 2;
                ++j;
            }
        }
    }
}

void precompute() {
    precompute_macro();
    
    //precomputing micro
    fill(start, start+(1<<(2*(B-1))), -1);
    for (int u=0; u<n; ++u) {
        root[u] = -1; // denotes that the vertex is not in a micro-tree
        if (size[u] <= B) { // at a micro-vertex
            if (u == r || size[parent[u]] > B) { // u should be the root of a micro-tree
                root[u] = u;
            } else { // we write root of the micro-tree where u is
                root[u] = root[parent[u]];
            }
        }
        if (root[u] == u) { // u is the root of a micro-tree
            isomorph(u); // compute the bijection to isomorphic tree
            code[u] = euler_code(u); // compute the euler code to identify the subtree class
            if (start[code[u]] == -1) { // the table hasn't been computed for that subtree class
                compute_table(u);
            }
        }
    }
}

int query_macro_tree(int u, int d) {
    int l = leaf[u];
    d += depth[l] - depth[u];
    if (d == 0) {
        return u;
    }
    if (d > n-1) {
        return r;
    }
    int j = logtable[d];
    int v = jump[leafnum[l]][j];
    d -= powtable[j];
    return paths[index[v] - d];
}

int query(int u, int d) {
    if (root[u] != -1) { //micro
        int r = root[u];
        if (depth[u] - depth[r] > d) {
            int i = f[u];
            if (u == r) {
                i = 0;
            }
            int k = code[r];
            int w = table[start[k] + size[r]*i + d];
            return f1[f[r] + w];
        }
        return query_macro_tree(parent[r], d - (depth[u]-depth[r]+1));
    } 
    return query_macro_tree(u, d);
}

int main() {
    cin >> n;
    B = logtable[n]/4;

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
