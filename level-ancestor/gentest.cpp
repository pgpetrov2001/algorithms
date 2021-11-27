#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <vector>
using namespace std;

const int MAXN = 1e7;
vector<int> children[MAXN];
bool haschildpath[MAXN];
int depth[MAXN];
int parent[MAXN];
vector<vector<int>> paths;
int pathof[MAXN];
int n;

vector<pair<int,int>> edges;

void gentree() {
    puts("generating tree");
    paths.push_back({0});
    pathof[0] = 0;
    int maxd = 0;
    for (int v=1; v<n; v++) {
        int p = rand()%v;
        children[p].push_back(v);
        if (!haschildpath[p]) {
            haschildpath[p] = true;
            pathof[v] = pathof[p];
        } else {
            pathof[v] = paths.size();
            paths.push_back({});
        }
        paths[pathof[v]].push_back(v);
        depth[v] = depth[p]+1;
        maxd = max(maxd, depth[v]);
        parent[v] = p;
        edges.push_back({p,v});
    }
    printf("tree has depth %d\n", maxd);
    puts("generated tree");
}

void gentree1() {
    paths.push_back({0});
    pathof[0] = 0;
    int maxd = 0;
    fill(component, component+n, -1);
    for (int e=0; e<n-1; ++e) {
        int u, v;
        do {
            u = rand()%n;
            v = rand()%n;
        } while (u == v || (component[u] != -1 && component[u] == component[v]));
        if (component[u] == -1) {
            component[u] = u;
        }
        component[v] = u;
        edges.push_back({u,v});
        adj[u].push_back(v);
        adj[v].push_back(u);
        if (!haschildpath[u]) {
            haschildpath[u] = true;
            pathof[v] = pathof[u];
        } else {
            pathof[v] = paths.size();
            paths.push_back({});
        }
        paths[pathof[v]].push_back(v);
        depth[v] = depth[p]+1;
        maxd = max(maxd, depth[v]);
        parent[v] = p;
    }
}

const int MAXQ = 1e7;
int q1[MAXQ], q2[MAXQ];
int ans[MAXQ];

void genqueries(int q) {
    puts("generating queries");
    for (int i=0; i<q; ++i) {
        int u = rand()%n;
        int v = u;
        int d = 0;
        while (d < 0 || (depth[v] && rand()%10<7)) {
            auto& pa = paths[pathof[v]];
            int ind = depth[v]-depth[pa[0]];
            v = parent[pa[0]];
            d += ind+1;
        }
        auto& pa = paths[pathof[v]];
        int ind = depth[v]-depth[pa[0]];
        int anc = pa[rand()%(ind+1)];
        d += depth[v] - depth[anc];
        q1[i] = u;
        q2[i] = d;
        ans[i] = anc;
    }
    puts("generated queries");
}

int main(int argc, char *argv[]) {
    n = stoi(argv[1]);
    int q = stoi(argv[2]);
    cout << n << " " << q << endl;
    gentree();
    genqueries(q);

    std::random_shuffle(edges.begin(), edges.end());

    
    ofstream infile(string(argv[3])+".in");
    ofstream outfile(string(argv[3])+".out");

    infile << n << endl;
    for (auto& e : edges) {
        infile << e.first << " " << e.second << "\n";
    }

    infile << q << endl;
    for (int i=0; i<q; ++i) {
        infile << q1[i] << " " << q2[i] << "\n";
        outfile << ans[i] << "\n";
    }
}
