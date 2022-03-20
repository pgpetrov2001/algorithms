#include <iostream>
#include <fstream>
#include <cstring>
/* #include <map> */
#include <vector>
#include <stack>
#include <cassert>

/* #include "arrmap.h" */
#include "vector.h"

/* using std::map; */
/* using std::vector; */

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::stack;

class Delta {
    class Indexer {
        //TODO: maybe make public to aid compiler optimization
        Delta &parent;
        int q;

        int &push_back(char a) {
            parent.next.push_back(0);
            parent.key.push_back(a);
            parent.val.push_back(0);
            return parent.val.back();
        }
        int &push_back(char a, int val) {
            parent.next.push_back(0);
            parent.key.push_back(a);
            parent.val.push_back(val);
            return parent.val.back();
        }

    public:
        Indexer(Delta &parent, int q) : parent(parent), q(q) {}

        int &operator[](char a) {
            if (!parent.head[q]) {
                parent.head[q] = parent.next.size();
                return push_back(a);
            }
            int prev;
            int tr = parent.head[q];
            while (tr) {
                if (parent.key[tr] == a) 
                    return parent.val[tr];
                prev = tr;
                tr = parent.next[tr];
            }
            parent.next[prev] = parent.next.size();
            return push_back(a);
        }

        //!!! assumption they have the same parent
        //TODO: make value maybe
        void operator=(const Indexer &other) {
            int tr_other = parent.head[other.q];
            if (!tr_other) 
                return;
            parent.head[q] = parent.next.size();
            push_back(parent.key[tr_other], parent.val[tr_other]);
            int tr = parent.head[q];
            tr_other = parent.next[tr_other];
            while (tr_other) {
                parent.next[tr] = parent.next.size();
                push_back(parent.key[tr_other], parent.val[tr_other]);
                tr = parent.next[tr];
                tr_other = parent.next[tr_other];
            }
        }
    };

public:
    Delta() = default;
    Delta(int n) {
        resize(n);
    }
    void resize(int n) {
        head.resize(2*n/*, 0*/);

        next.reserve(3*n);
        key.reserve(3*n);
        val.reserve(3*n);

        next.resize(1); //because 0 means null
        key.resize(1);
        val.resize(1);
    }
    Indexer operator[](int q) {
        return Indexer{*this, q};
    }
    int size() const {
        return int(next.size()) - 1;
    }
    void clear() {
        head = {};
        next = {};
        val = {};
        key = {}; //TODO: maybe remove for performance
    }

private:
    vector<int> head;
    vector<int> next;
    vector<char> key;
    vector<int>  val;
};

struct state {
    int len;
    int pos;
};

vector<int> link;
/* constexpr int bsz = 2*12252240; */
/* constexpr int bsz = 16; */
/* allocator<bsz> my_allocator; */
/* vector<map<char,int,bsz,my_allocator>> delta; */
/* vector<map<char,int>> delta; */
Delta delta;

const int MAXN = 2e8;
vector<char> word;
int n;

int qn, dn;
int q_last;
vector<state> Q;

int redirect(int q, int q_dest, int q_old_dest, char a) {
    //TODO: save delta[q][a]
    while (q && delta[q][a] == q_old_dest) {
        /* if (!delta[q][a]) */
        /*     ++dn; */
        delta[q][a] = q_dest;
        q = link[q];
    }
    return q;
}

void create_new_state(int q_betak, int q_gamma, char a) {
    int q_new = qn++;
    delta[q_betak][a] = q_new;
    Q[q_new].len = Q[q_betak].len + 1;
    Q[q_new].pos = Q[q_gamma].pos;
    link[q_new] = link[q_gamma];
    delta[q_new] = delta[q_gamma];
    /* dn += delta[q_new].size(); */
    link[q_gamma] = q_new;
    link[q_last] = q_new;
    redirect(link[q_betak], q_new, q_gamma, a);
}

void insert_el(char a) {
    int q_prev = q_last;
    q_last = qn++;
    Q[q_last].len = Q[q_prev].len + 1;
    Q[q_last].pos = Q[q_prev].len;
    int q_betak = redirect(q_prev, q_last, 0, a);
    if (!q_betak) {
        link[q_last] = 1;
        return;
    }
    int q_gamma = delta[q_betak][a];
    if (Q[q_betak].len + 1 == Q[q_gamma].len) {
        link[q_last] = q_gamma;
        return;
    }
    create_new_state(q_betak, q_gamma, a);
}

void create_first_state() {
    Q.resize(2*n);
    /* cout << "created q " << endl; */
    link.resize(2*n);
    /* cout << "created link " << endl; */
    /* delta.resize(2*n); */
    delta.resize(n);
    /* cout << "created delta " << endl; */
    Q[1].len = 0;
    link[1] = 0;
    q_last = 1;
    qn = 2;
    /* cout << "created" << endl; */
}

//TODO: mind memory consumption
//below are max 2*MAXN
/* vector<small_vector_int> children; */ 
vector<vector<int>> children;
vector<int> curr_child;
vector<int> entry_time;
vector<int> exit_time;
vector<int> prefix_ending_at;
int TM;

void dfs_times() {
    stack<int> to_visit;
    to_visit.push(1);
    while (to_visit.size()) {
        int q = to_visit.top();
        if (curr_child[q] == 0) {
            ++TM;
            entry_time[q] = TM;
        }
        if (curr_child[q] < children[q].size()) {
            to_visit.push(children[q][curr_child[q]++]);
        } else {
            ++TM;
            exit_time[q] = TM;
            to_visit.pop();
        }
    }
}

int dfs_solve() {
    std::fill(curr_child.begin(), curr_child.end(), 0);
    int ans = 0;
    stack<int> to_visit;
    vector<int> parent_with_len(n+1);
    to_visit.push(1);
    while (to_visit.size()) {
        int q = to_visit.top();
        if (curr_child[q] == 0) {
            if ((Q[q].len & 1) == 0) {
                int q_hl = parent_with_len[Q[q].len / 2];
                if (q_hl) {
                    int q_pref = prefix_ending_at[Q[q].pos - Q[q_hl].len];
                    if (entry_time[q_pref] >= entry_time[q_hl] && exit_time[q_pref] <= exit_time[q_hl]) 
                        ++ans;
                }
            }
            parent_with_len[Q[q].len] = q;
        }
        if (curr_child[q] < children[q].size()) {
            to_visit.push(children[q][curr_child[q]++]);
        } else {
            parent_with_len[Q[q].len] = 0;
            to_visit.pop();
        }
    }
    return ans;
}

int solve() {
    /* int check = 0; */
    /* for (int q=1; q<qn; ++q) */
    /*     check += delta[q].size(); */
    /* assert(check == dn); */
    /* my_allocator.purge_memory(); */
    dn = delta.size();
    delta.clear();
    /* assert(delta.size() == dn); */
    /* cout << "cleared delta " << endl; */
    {
        children.resize(qn+1);
        std::vector<bool> marked(qn+1);
        for (int q=1; q<qn; ++q) {
            for (int q1=q; q1 && !marked[q1]; q1=link[q1]) {
                marked[q1] = true;
                children[link[q1]].push_back(q1);
            }
        }
        link.clear();
    }
    prefix_ending_at.resize(n);
    for (int q=1; q<qn; ++q)
        if (Q[q].pos + 1 == Q[q].len) 
            prefix_ending_at[Q[q].pos] = q;
    curr_child.resize(qn+1);
    entry_time.resize(qn+1);
    exit_time.resize(qn+1);
    dfs_times();
    return dfs_solve();
}

int main(int argc, char *argv[]) {
    ifstream file(argv[1], std::ios::binary);
    file.seekg (0, std::ios::end);
    n = file.tellg();
    word.resize(n+1);
    file.seekg (0, std::ios::beg);
    file.read(&word[0], n);

    create_first_state();
    for (int i=0; i<n; ++i) 
        insert_el(word[i] - 'a');
    /* cout << "constructed" << endl; */

    int fn = 0;
    for (int q=q_last; q; q=link[q])
        ++fn;

    int ans = solve();

    cout << qn-1  << endl;
    cout << dn    << endl;
    cout << fn    << endl;
    cout << ans+1 << endl;
    exit(0);
}
