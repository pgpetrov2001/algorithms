#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <stack>

#include "../RMQ/run-time-mem/+1-1/n_1.hpp"

using namespace std;

const int SIGMA = 2; // TODO: change to 26 for example

vector<array<int, SIGMA>> delta{{}};
vector<bool> F{false};

int add_state() {
    int ret = delta.size();
    delta.push_back({});
    F.push_back(false);
    return ret;
}

int get_root() {
    return 1;
}

int num_vertices() {
    return (int)delta.size() - 1;
}

int last_state() {
    return get_root() + num_vertices() - 1;
}

void build_trie(const vector<string> &D) {
    add_state(); // root
    for (const string &w : D) {
        int v = get_root();
        for (char c : w) {
            c -= 'a';
            if (!delta[v][c]) {
                delta[v][c] = add_state();
            }
            v = delta[v][c];
        }
        F[v] = true;
    }
}

vector<vector<int>> children_of{{}};
vector<int> depth{0};
vector<int> real_depth{0};

void add_child(int u, int v) { // assumes that will be called with all children of u consecutively
    assert(v == depth.size());
    children_of.emplace_back();
    depth.push_back(depth[u] + 1);
    real_depth.push_back(real_depth[u] + 1);
    children_of[u].push_back(v);
}

vector<int> euler_tour;
vector<int> euler_tour_depths; // could be avoided
vector<int> index;

void dfs_euler(int u) {
    index[u] = euler_tour.size();
    euler_tour.push_back(u);
    euler_tour_depths.push_back(depth[u]);
    for (int v : children_of[u]) {
        dfs_euler(v);
        euler_tour.push_back(u);
        euler_tour_depths.push_back(depth[u]);
    }
}

void compute_euler_tour() {
    euler_tour.reserve(2 * (int)children_of.size() - 1);
    euler_tour_depths.reserve(2 * (int)children_of.size() - 1);
    index.resize(children_of.size());
    dfs_euler(0);
}

vector<int> equiv_class_of;
optional<RMQ_1<int>> rmq;
int maxlen;

void precompute(const vector<string> &D) {
    for (const string &w : D) {
        maxlen = max((int)w.size(), maxlen);
    }

    vector<vector<int>> curr_equiv_classes[2]; 
    vector<int> curr_equiv_class_of; // prev letter and current letter
    vector<int> prev_equiv_class_of;
    vector<int> parent_class[2];
    vector<int> new_class_from_old;
    equiv_class_of.resize(last_state()+1);

    curr_equiv_classes[0].reserve(last_state()+1); // maximum number of equivalence classes
    curr_equiv_classes[1].reserve(last_state()+1);
    curr_equiv_class_of.resize(last_state()+1);
    prev_equiv_class_of.resize(last_state()+1);
    parent_class[0].reserve(last_state()+1);
    parent_class[1].reserve(last_state()+1);
    new_class_from_old.resize(last_state()+1);

    add_child(0, 1);
    add_child(0, 2);

    int last_class = 2;
    curr_equiv_classes[1-SIGMA%2].emplace_back();
    curr_equiv_classes[1-SIGMA%2].emplace_back();
    parent_class[1-SIGMA%2].push_back(1);
    parent_class[1-SIGMA%2].push_back(2);

    for (int v=get_root(); v<=last_state(); ++v) {
        int cl = curr_equiv_class_of[v] = int(F[v]);
        curr_equiv_classes[1-SIGMA%2][cl].push_back(v);
    }
    curr_equiv_class_of[0] = 0;
    curr_equiv_classes[1-SIGMA%2][0].push_back(0); // error state

    //create a minimization tree
    //where each level k represents the equivalence classes on the k-th iteration
    //from the standard automaton minimization algorithm, namely:
    //2 classes are equivalent if they have the same right context for a length up to k-1
    //each classes children are the equivalence classes that it splits into on the next iteration
    for (int l=1; l<=maxlen; ++l) {
        swap(prev_equiv_class_of, curr_equiv_class_of);
        swap(curr_equiv_classes[1], curr_equiv_classes[1-SIGMA%2]);
        swap(parent_class[1], parent_class[1-SIGMA%2]);

        //TODO: Optimize SIGMA*Pref(D) to O(log(SIGMA) + log(log(Pref(D)))) somehow with veb trees
        //where l(D) = maxlen
        //SIGMA is a hidden constant
        for (int c=0; c<SIGMA; ++c) {
            //split the classes for each letter separately
            //and then add the edges only for the end result
            curr_equiv_classes[c%2].clear();
            parent_class[c%2].clear();
            fill(new_class_from_old.begin(), new_class_from_old.end(), -1);

            assert(curr_equiv_classes[1-c%2].size());
            int first_class = 0; 
            int curr_class = 0;
            //the nested for loops are O(Pref(D)) in total
            for (const auto &eq_class : curr_equiv_classes[1-c%2]) {
                for (int v : eq_class) {
                    int u = delta[v][c];
                    int cl = prev_equiv_class_of[u];

                    if (new_class_from_old[cl] < first_class) { // haven't created yet
                        new_class_from_old[cl] = curr_equiv_classes[c%2].size();
                        curr_equiv_classes[c%2].emplace_back();
                        parent_class[c%2].push_back(parent_class[1-c%2][curr_class]);
                    }
                    curr_equiv_class_of[v] = new_class_from_old[cl];
                    curr_equiv_classes[c%2][new_class_from_old[cl]].push_back(v);
                }

                first_class = curr_equiv_classes[c%2].size();
                ++curr_class;
            }
        }

        //construct edges in tree, but keep it compressed, meaning:
        //a downwards path of vertices, without branches is represented only by the lowest vertex
        //that way the memory footprint of storing that tree is:
        //O(|leaves|) instead of O(|leaves|*(tree height))
        stack<int> children;
        const auto add_children = [&](int prev_parent, int curr_parent) {
            if (prev_parent != curr_parent) {
                if (children.size() > 1) {
                    while (!children.empty()) {
                        add_child(prev_parent, ++last_class);
                        parent_class[1-SIGMA%2][children.top()] = last_class;
                        children.pop();
                    }
                } else { // == 1
                    ++real_depth[prev_parent];
                    parent_class[1-SIGMA%2][children.top()] = prev_parent;
                    children.pop();
                }
            }
        };

        int prev_parent = parent_class[1-SIGMA%2][0];
        for (int curr_class = 0; curr_class < curr_equiv_classes[1-SIGMA%2].size(); ++curr_class) {
            int curr_parent = parent_class[1-SIGMA%2][curr_class];
            add_children(prev_parent, curr_parent);
            children.push(curr_class);
            prev_parent = curr_parent;
        }
        add_children(prev_parent, -1);
    }
    int curr_class = 0;
    for (const auto &eq_class : curr_equiv_classes[1-SIGMA%2]) {
        for (int v : eq_class) {
            equiv_class_of[v] = parent_class[1-SIGMA%2][curr_class];
        }
        ++curr_class;
    }
    compute_euler_tour();
    rmq.emplace(euler_tour_depths);
}

int LCA(int cl1, int cl2) {
    return euler_tour[rmq->query(index[cl1], index[cl2])];
}

int query(int u, int v) {
    int cl_u = equiv_class_of[u];
    int cl_v = equiv_class_of[v];
    return real_depth[LCA(cl_u, cl_v)];
}

int follow_trie(const string &w) {
    int v = get_root();
    for (char c : w) {
        c -= 'a';
        v = delta[v][c];
    }
    return v;
}

int main() {
    int n;
    cin >> n;

    vector<string> D(n);

    for (int i=0; i<n; ++i) {
        cin >> D[i];
    }

    build_trie(D);
    precompute(D);

    int q;
    cin >> q;

    while (q--) {
        string a, b;
        cin >> a >> b;

        int va = follow_trie(a);
        int vb = follow_trie(b);

        cout << query(va, vb) << endl;
    }
}
