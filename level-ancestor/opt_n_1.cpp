#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

const int MAXN = 2e6, MAXQ = 2e5;
const int MAXLOGN = 21;
const int MAXB = MAXLOGN/2;
const int MAXISOMORPH = 1<<(2*(MAXB-1));
const int MAXLEAVES = MAXN/(MAXB+1);

//basic stuff
int logtable[MAXN+1];
int size[MAXN], leaf[MAXN], parent[MAXN], depth[MAXN];
//path decomposition stuff and jump stuff
int index[MAXN], tobegin[MAXN];
int paths[MAXN*2], leafnum[MAXN], jump[MAXLEAVES][MAXLOGN];
vector<int> children[MAXN];
vector<int> leavesatdepth[MAXN];
int root[MAXN], f1[MAXN], f[MAXN], code[MAXN];
int table[MAXB*MAXB*MAXISOMORPH];
int start[MAXISOMORPH];

int B;
int n, r=0;
int path_ind = 0;

void dfs(int u) {
    size[u] = 1;
    leaf[u] = -1;
    for (int v : children[u]) {
        parent[v] = u;
        depth[v] = depth[u]+1;
        dfs(v);
        if (size[v] > B) {
            leaf[u] = leaf[v];
        }
        size[u] += size[v];
    }
    if (leaf[u] == -1 && size[u] > B) {
        leaf[u] = u;
    }
}

void precompute_pows_and_logs() {
    int log = 0, pow = 2;
    for (int num=1; num<=n; ++num) {
        if (num >= pow) {
            ++log;
            pow *= 2;
        }
        logtable[num] = log;
    }
}

int get_max_path_len_from_leaf(int l) {
    int v = l, w = l; // w is the vertex below v
    int max_path_len = 0;
    do {
        index[v] = 0;
        ++max_path_len;
        w = v;
        v = parent[v];
    // we don't go in when the vertex below v is the root of tree or when v is marked
    } while (w != r && index[v] == -1);
    return max_path_len;
}

void precompute_path_from_leaf(int l) {
    //Now we write the vertices from the doubled path in the paths array 
    //and number of edges to the begin of the corresponding path
    int max_path_len = get_max_path_len_from_leaf(l);
    int double_path_len = max_path_len * 2 - 1;
    int v = l;
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
    int L = leafnum[l];
    int v = parent[l];
    jump[L][0] = v;
    int j = 1, d = 1;
    while (d*2 <= depth[l]) { // n-1 is the maximum jump
        v = paths[index[v] - d];
        jump[L][j] = v;
        d *= 2;
        ++j;
    }
}

void precompute_macro_tree() {
    precompute_pows_and_logs();
    parent[r] = r;  
    depth[r] = 0;
    dfs(r);
    for (int u=0; u<n; ++u) {
        if (leaf[u] == u) {
            leavesatdepth[depth[u]].push_back(u);
        }
    }
    int L = 0;  
    fill(index, index+n, -1);
    for (int d=n-1; d>=0; --d) 
        for (int l : leavesatdepth[d]) {
            leafnum[l] = L++;
            precompute_path_from_leaf(l);
            precompute_jumps_from_leaf(l);
        }
}

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
        dfs_table(c);
    }
}

void compute_table(int r) {
    start[code[r]] = ind_table;
    fill(&table[ind_table], &table[ind_table + size[r]], 0);
    int index_in_f1 = f[r];
    f[r] = 0; 
    // to not mess up the assumption that the parent's f is it's number in the
    // tree
    for (int c : children[r]) {
        dfs_table(c);
    }
    f[r] = index_in_f1;
    ind_table += size[r]*size[r];
}

int num;
void dfs_isomorph(int u) { 
    f[u] = num;
    f1[indf1 + num] = u;
    ++num;
    for (int c : children[u]) {
        dfs_isomorph(c);
    }
}

void isomorph(int r) {
    num = 0;
    dfs_isomorph(r);
    f[r] = indf1;
    indf1 += size[r];
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
void dfs_assign(int u) {
    if (root[parent[u]] != -1) {
        root[u] = root[parent[u]];
    }
    for (int c : children[u]) {
        dfs_assign(c);
    }
}

void precompute() {
    precompute_macro_tree();
    
    fill(root, root+n, -1);
    for (int u=0; u<n; ++u) {
        if (size[u] <= B && size[parent[u]] > B) {  
            root[u] = u;
            dfs_assign(u);
        }
    }
    fill(start, start+(1<<(2*(B-1))), -1);
    for (int u=0; u<n; ++u) 
        if (root[u] == u) {
            isomorph(u);  
            code[u] = euler_code(u);  
            if (start[code[u]] == -1) {  
                compute_table(u);
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
    d -= 1<<j;
    return paths[index[v] - d];
}

int query(int u, int d) {
    if (root[u] != -1) { //micro
        int r = root[u];
        if (depth[u] - depth[r] >= d) {
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
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n;
    precompute_pows_and_logs();
    B = logtable[n]/3;

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
        cout << query(u,d) << endl;
    }
}
