#include <iostream>
#include <queue>
#include <vector>
using namespace std;

const int maxn = 1e5, maxm = 2e5, maxq = 2e5;
vector<int> adj[maxn];

int main() {
	int n, m, q;
	cin >> n >> m >> q;

	for (int i=0; i<m; ++i) {
		int u, v;
		cin >> u >> v;
		--u; --v;
		adj[u].push_back(v);
		adj[v].push_back(u);
	}

	while (q--) {
		int s, t;
		cin >> s >> t;
		--s; --t;

		queue<int> q;
		q.push(s);
		vector<int> dist(n, n+1);
		dist[s] = 0;
		while (!q.empty()) {
			int v = q.front();
			if (v == t) break;
			q.pop();
			for (int u : adj[v]) {
				if (dist[u] == n+1) {
					dist[u] = dist[v] + 1;
					q.push(u);
				}
			}
		}

		cout << dist[t] << endl;
	}
}
