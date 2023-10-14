#include <iostream>
#include <vector>
#include <queue>
#include <cassert>
#include <algorithm>
#include <stack>
using namespace std;

#if (false || defined(DEBUG)) && !defined(EVAL)
    #include "debug.h" // https://github.com/kiprasmel/debug.h
#else
    struct debug {
        template <class c> debug& operator <<(const c&) { return * this; }
    };
    #define imie(...) ""
#endif

const int maxn = 2e6+10;
vector< pair<int,int> > adj[maxn]; // m=1e5, n + 2 * 5 * m = 1100000
typedef long long ll;
ll dp[maxn];
const ll mod = 1e9+7;
bool vis[maxn];

int CURR_VERTEX;

void add_edge(int u, int v, int w) {
	int curr_v = u;

	stack<int> digits;
	while (w > 0) {
		digits.push(w%10);
		w /= 10;
	}

	while (digits.size()) {
		int digit = digits.top();
		digits.pop();
		int next_vertex;
		if (digits.size() == 0) {
			next_vertex = v;
		} else {
			next_vertex = CURR_VERTEX++;
		}
		adj[curr_v].push_back({next_vertex, digit});
		curr_v = next_vertex;
	}
}

int main()
{
	int n, m;
	cin >> n >> m;
	CURR_VERTEX = n+1;

	for (int i=0; i<m; ++i) {
		int u, v;
		cin >> u >> v;
		add_edge(u, v, i+1);
		add_edge(v, u, i+1);
	}

	queue< pair<vector<int>, ll> > q;
	q.push({{1}, 0});
	vis[1] = true;

	while (!q.empty()) {
		auto [ group, curr_dp ] = q.front();
		q.pop();

		vector<pair<int,int>> edges;
		for (int v : group) {
			for (auto [u,w] : adj[v]) {
				if (vis[u]) continue;
				edges.push_back({w,u});
			}
		}

		sort(edges.begin(), edges.end());
		int curr_w = -1;
		vector<int> curr_group;
		for (auto [w,u] : edges) {
			if (vis[u]) continue;
			vis[u] = true;
			dp[u] = (curr_dp*10+w)%mod;

			if (w > curr_w) {
				if (curr_w > -1) { // this was the bug here, in the new graph we have edges with 0 weight, although this doesn't break our assumption that paths starting from 1 don't have leading zeros
					assert(curr_group.size());
					q.push({
						curr_group,
						dp[curr_group[0]]
					});
				}
				curr_w = w;
				curr_group.clear();
			}
			curr_group.push_back(u);
		}
		if (edges.size()) {
			assert(curr_group.size());
			q.push({
				curr_group,
				dp[curr_group[0]]
			});
		}

	}

	for (int i=2; i<=n; ++i) {
		cout << dp[i] << " ";
	}
	cout << endl;
}
