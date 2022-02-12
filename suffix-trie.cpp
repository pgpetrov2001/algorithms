#include <iostream>
#include <string_view>
#include <map>
#include <vector>

using namespace std;

const int MAXN = 1e6;

struct State {
    int pos;
    int len;
    int s_link = 0; // means null state
} Q[2*MAXN];

map<int, int> delta_[26];

int &delta(int q, char a) {
    return delta_[a][q];
}

bool has_trans(int q, char a) {
    return delta_[a].count(q);
}

int count_trans(int q) {
    int ans = 0;
    for (char a=0; a<26; ++a) {
        ans += delta_[a][q].size();
    }
    return ans;
}

int get_len(int q) {
    if (count_trans(q) == 0) {
        //contains data about the first state along the path from its parent to q
        //this state is for a string that is encountered once as a proper infix (if it's different from q)
        //and possibly once as a suffix, therefore pos is correct for q
        //q is a suffix
        return n - Q[q].pos;
    }
    return Q[q].pos;
}

//to be able to get s_link of leaves we need to store additionally parent of every node

int qn = 1;
int n = 0;
char word[MAXN];

string_view get_str(int q) {
    return {word + Q[q].pos, get_len(q)};
}

struct ImplicitState;

ImplicitState follow_word(int q, int k, int pos) {
    if (k == 0) { // covers additional case for leaves
        return {q};
    }
    char a = word[pos];
    if (has_trans(q, a)) {
        return {0};
    }
    int p = delta(q, a);
    int jump = Q[p].len - Q[q].len;
    if (k < jump) {
        return {q, k, a};
    }
    return follow_word(p, k - jump, pos + jump);
}

struct ImplicitState {
    int q;
    int k = 0;
    char a = '\0';

    operator bool () const {
        return q;
    }

    ImplicitState follow_char(char b) const {
        if (k == 0) {
            if (!has_trans(q, b)) {
                return {0};
            }
            int p = delta(q, b);
            if (Q[p].len - Q[q].len == 1) {
                return {p};
            }
            return {q, 1, b};
        }
        int p = delta(q, a);
        if (word[Q[p].pos + Q[q].len + k] != b) {
            return {0};
        }
        if (k == Q[p].len - Q[q].len - 1) {
            return {p};
        }
        return {q, k+1, a};
    }

    ImplicitState get_s_link() const {
        if (Q[q].len == 0) {
            assert(k);
            int p = delta(q, a);
            int i = Q[p].pos + 1;
            return follow_word(q, k-1, i);
        } 
        if (k == 0) {
            return {Q[q].s_link};
        }
        int l = Q[q].s_link;
        int p = delta(q, a);
        int i = Q[p].pos + Q[q].len + 1;
        return follow_word(l, k, i);
    }
};


ImplicitState stem;

void add_char(char c) {
    c -= 'a';

    int prev_stem = 0;
    while (stem && !follow_char(stem, c)) {
        //new states
        if (stem.k) {
            int p = delta[stem.a][stem.q];
            Q[qn] = {Q[p].pos, Q[q].len + stem.k}; // compute suffix link later
            delta[stem.a][stem.q] = qn;
            char a = word[Q[qn].pos + Q[qn].len];
            delta(qn, a) = p;
            if (prev_stem) {
                Q[prev_stem].s_link = qn;
            }
            stem = {qn};
            qn++;
        }
        //create destination state
        Q[qn] = {Q[stem.q].pos, Q[stem.q].len + 1};
        delta[c][stem.q] = qn;
        qn++;

        prev_stem = stem.q;
        stem = stem.get_s_link();
    }

    if (prev_stem) {
        Q[prev_stem].s_link = stem;
        stem = ImplicitState{stem.q}.follow_char(c); // not a leaf because proper infix
    }

    word[n++] = c;
}

int main() {
    cin >> word;
    int n = strlen(word);
    for (int i=0; i<n; ++i) {
        add_char(word[i]);
    }
}
