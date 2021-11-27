#include <iostream>
#include <vector>
using namespace std;

const int MAXSZ = 2e6; // maxmimum size of tree = 2*10^5
vector<int> children[MAXSZ]; // a vector of the adjacent vertices of a vertex
int depth[MAXSZ];
int parent[MAXSZ];

bool marked[MAXSZ];
vector<int> vertices_with_depth[MAXSZ];

void dfs(int u) {
    vertices_with_depth[depth[u]].push_back(u);
    for (int child : children[u]) {
        // if the vertex in the adjacent list is not a parent of the node then it is its child
        parent[child] = u;
        depth[child] = depth[u] + 1;
        dfs(child);
    }
}

vector<int> paths_array; // we write all paths one after another in one array
int index_begin_of[MAXSZ]; // index(in paths_array) of the leaf of the path that the vertex is in
int index_of[MAXSZ]; // index of the vertex in the path array

int query(int u, int k) { // returns level k ancestor of the vertex u
    if (depth[u] == 0) { // at the root
        return u;
    }
    int index_in_path = index_of[u] - index_begin_of[u];
    if (k <= index_in_path) { // ancestor is in the current path
        return paths_array[index_of[u] - k];
    }
    // else we try one path upwards
    int v = paths_array[index_begin_of[u]];
    return query(parent[v], k - (index_in_path+1));
}

int child[MAXSZ];

int main() {
    ////cout << "Type the tree size, denoted as n:" << endl;
    int tree_sz;
    cin >> tree_sz;

    ////cout << "Type the n-1 edges of the tree in the format \"[vertex] [vertex]\" where the vertices are labeled from 0 to n-1:" << endl;
    for (int i=0; i<tree_sz-1; ++i) {
        int u, v; // edge between vertices u and v
        cin >> u >> v; // numbers of vertices are from 0 to tree_sz
        children[u].push_back(v); 
        // we add the edge in both directions
    }

    for (int i=0; i<tree_sz; ++i) {
        marked[i] = false;
    }

    //let the root be 0
    //its parent will be 0
    depth[0] = 0;
    parent[0] = 0;
    dfs(0); // calculates depth and parent of every vertex

    //algorithm for building the paths (build upwards starting from leaves), leaves in descending order of depth
    for (int d=tree_sz-1; d>=0; --d) // d is for depth
        for (int u : vertices_with_depth[d]) 
            if (!marked[u]) { // unmarked vertices that are leaves will be visited before vertices that are not
                int v = u, x = u; // v is the current vertex and x is the child of v
                while (!marked[v]) { // the parent of the root is the root, and therefore will be marked on the second attempt
                    marked[v] = true;
                    child[v] = x;
                    x = v;
                    v = parent[v];
                }
                // x is the highest vertex of the chain and u is the lowest
                int w = x; // w is the current vertex while going down the chain
                while (child[w] != w) { // we have set u to be its own child, that way we signify that u has no child
                    // we push them in reverse order in the paths array so that shallower vertices are with lower indices
                    index_of[w] = paths_array.size();
                    paths_array.push_back(w);
                    index_begin_of[w] = index_of[w] - depth[w] + depth[x];
                    w = child[w];
                } 
                index_of[u] = paths_array.size();
                paths_array.push_back(u);
                index_begin_of[u] = index_of[u] - depth[u] + depth[x];
            }

    //cout << "The paths array is the following:" << endl;
    //for (int v : paths_array) {
        //cout << v << " ";
    //}
    //cout << endl;

    //cout << "Type the number of queries you want to ask:" << endl;
    int q; 
    cin >> q;

    //cout << "Queries are in the format \"[vertex] [level]\":" << endl;

    while (q--) {
        int u, k; 
        cin >> u >> k;

        cout << query(u, k) << endl;
    }
}
