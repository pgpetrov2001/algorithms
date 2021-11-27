#include <iostream>
#include <queue>
#include <vector>
using namespace std;

const int MAXN = 2e5;
const int INF = 1e9;

vector<int> edges[MAXN+1];
int dist[MAXN+1];
int pairU[MAXN+1];
int pairV[MAXN+1];
int n, m, k;

bool bfs_find_augment() {
    queue<int> q;

    for (int u=1; u<=n; ++u) {
        if (!pairU[u]) {
            dist[u] = 0;
            q.push(u);
        } else {
            dist[u] = INF;
        }
    }

    dist[0] = INF;
    while (q.size()) {
        int u = q.front();
        q.pop();
        if (dist[u] < dist[0]) {
            for (int v : edges[u]) {
                if (dist[pairV[v]] == INF) {
                    dist[pairV[v]] = dist[u]+1;
                    q.push(pairV[v]);
                }
            }
        }
    }

    return dist[0] < INF;
}

bool dfs_augment(int u) {
    if (!u)
        return true;
    for (int v : edges[u]) {
        if (dist[pairV[v]] == dist[u]+1) {
            if (dfs_augment(pairV[v])) {
                pairU[u] = v;
                pairV[v] = u;
                return true;
            }
        }
    }
    dist[u] = INF;
    return false;
}

bool taken[MAXN+1];
bool visited[MAXN+1];

void walk_alternate(int u) {
    visited[u] = true;
    for (int v : edges[u]) {
        if (!visited[v]) {
            visited[v] = true;
            walk(pairV[v]);
        }
    }
}

int main() {
    cin >> n >> m >> k;

    for (int i=0; i<k; i++) {
        int u, v;
        cin >> u >> v;
        edges[u].push_back(v);
        edges[v].push_back(u);
    }

    int size_match = 0;
    while (bfs_find_augment()) {
        for (int u=1; u<=n; ++u) {
            if (!pairU[u]) {
                if (dfs_augment(u)) {
                    ++size_match;
                }
            }
        }
    }

    //we visit all unmatched vertices and the ones connected to them by alternating paths
    for (int u=1; u<=n; ++u) {
        if (pairU[u] == 0) {
            walk_alternate(u);
        }
    }

    //Left part is U
    //Right part is V
    //each edge that has left endpoint in visited must also have a right endpoint in visited
    //the rest must have a left endpoint in U\visited
    //
    //1. the right endpoints in visited are matched because they are found by going along alternating paths
    //2. the left endpoints in U\visited are matched because if they were unmatched they would be in visited
    //from 1. and 2. => selected vertices are of exactly the same count as the number of matched edges,
    //which is the mimnimum count they could be

    for (int u=1; u<=n; ++u) {
        taken[u] = !visited[u];
    }
    for (int v=1; v<=m; ++v) {
        taken[v] = visited[v];
    }
}
