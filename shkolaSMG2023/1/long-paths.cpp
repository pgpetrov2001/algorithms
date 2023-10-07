#include <iostream>
#include <queue>
#include <vector>
using namespace std;

//Problem link: https://szkopul.edu.pl/problemset/problem/QHh99tBu4p9FMsFohv4da7S7/site/?key=statement
//in summary shortest path queries in unweighted undirected graph where we are guranteed that the answer to each query is at least n/10

const int maxn = 1e5, maxm = 2e5, maxq = 2e5;
vector<int> adj[maxn];

//idea for the solution is pretty gay
//the main observation is that if an arbitrary vertex x happens to lie on the shortest path between u and v
//we can calculate the answer as dist[u] + dist[v] where dist are the shortest distances from x
//if we consider that vertex to be random, by using the statement we conclude that it lies
//with probability 1/10 on the shortest path
//so if we make a sufficient amount of special vertices (denote it C) and run a BFS for each one
//the probability that at least one lies on the shortest path for a particular query
//is 1-(9/10)^C
//so by taking the min(x <- special_vertices, dist_x[u]+dist_x[v]) we obtain the answer for a single query
//because the formula for vertices not lying on a shortest path from the query will give an unoptimal answer
//
//so overall the success probability of our code is (1-(9/10)^C)^q
//
//>>> (1-(9/10)**300)**(2*10**5)
//0.9999999962474462
//
//here the choice of C has to be very careful, firstly because of this trend:
//>>> (1-0.9**130)**(2*10**5)
//0.7983614656393805
//>>> (1-0.9**120)**(2*10**5)
//0.5242166392532837
//>>> (1-0.9**100)**(2*10**5)
//0.00493032520029466
//
//however making C not that much bigger, although small enough not to cause a TLE, it still may cause a MLE

const int C = 300;
int dist[C][maxn];

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

	for (int i=0; i<C; ++i) {
		int v = rand()%n;
		queue<int> q;
		q.push(v);
		int inf = n+1;
		fill(dist[i], dist[i]+n, inf);
		dist[i][v] = 0;
		while (!q.empty()) {
			int v = q.front();
			q.pop();
			for (int u : adj[v]) {
				if (dist[i][u] == inf) {
					dist[i][u] = dist[i][v] + 1;
					q.push(u);
				}
			}
		}
	}

	while (q--) {
		int s, t;
		cin >> s >> t;
		--s; --t;
		int ans = n+1;
		for (int i=0; i<C; ++i) {
			int curr_dist = dist[i][s] + dist[i][t];
			ans = min(ans, curr_dist);
		}
		cout << ans << endl;
	}
}
