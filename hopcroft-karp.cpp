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
        //1)
        //ensures we move along the bfs graph and that we
        //apply augmentation only on vertex disjoint paths
        //because pairV[v] would be of distance dist[u], after a part of a path is already augmented
        if (dist[pairV[v]] == dist[u]+1) {
            if (dfs_augment(pairV[v])) {
                pairU[u] = v;
                pairV[v] = u;
                return true;
            }
        }
    }
    //2)
    //memoization
    dist[u] = INF;
    return false;
    //1) and 2) ensure that no vertex is visited twice
}

int main() {
    cin >> n >> m >> k;

    for (int i=0; i<k; i++) {
        int u, v;
        cin >> u >> v;
        edges[u].push_back(v);
        edges[v].push_back(u);
    }

    //taking the symmetric difference between an arbitrary matching M and the optimal matching M* ( the edges )
    //we get a graph P with every vertex having a degree at most 2
    //it consists of disjoint alternating paths with equal amount of edges of both kinds
    // or with alternating paths that start and end with an edge of M* ( a path with length 1 is also an alternating path)
    // since |M| < |M*| then there must be at least some alternating paths of the second kind 
    // therefore if a matching isn't optimal then there exists an alternating path within it 
    int size_match = 0;
    while (bfs_find_augment()) { // one iteration augments all augmenting paths of minimal length
        for (int u=1; u<=n; ++u) {
            if (!pairU[u]) {
                if (dfs_augment(u)) {
                    ++size_match;
                }
            }
        }
    }

    //after sqrt(n+m) iterations all augmenting paths are of length at least sqrt(n+m)
    //which means that there are at most sqrt(n+m) of them
    //observing the symmetric difference of the current matching with an optimal one
    //we can assert that there are at most sqrt(n+m) aug. paths in it
    //each such path contributes to the difference to the optimal matching by 1
    //so the current matching is not more than sqrt(n+m) edges away from the optimal one
    //and thus will take at most sqrt(m+n) more iterations to reach it
    //
    //therefore complexity is O(|E|sqrt(|V|))

    cout << size_match << endl;

    for (int u=1; u<=n; ++u) {
        if (pairU[u]) {
            cout << u << " " << pairU[u] << endl;
        }
    }
}
