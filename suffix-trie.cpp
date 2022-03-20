#include <iostream>
#include <string_view>
#include <map>
#include <vector>
#include <cstring>
#include <cassert>

using namespace std;

const int MAXN = 1e6;

struct State {
    int pos;
    int len;
    int s_link = 0; // means null state
} Q[2*MAXN];

map<int, int> delta_[26];

int &delta(int q, char a) {
    a -= 'a';
    return delta_[a][q];
}

bool has_trans(int q, char a) {
    a -= 'a';
    return delta_[a].count(q);
}

int count_trans(int q) {
    int ans = 0;
    for (char a=0; a<26; ++a) {
        ans += delta_[a].count(q);
    }
    return ans;
}

struct ImplicitState {
    int q;
    int k = 0;
    char a = '\0';

    operator bool () const {
        return q;
    }
    ImplicitState follow_char(char b) const;
    ImplicitState get_s_link() const;
};

int n = 0;
char word[MAXN];

int get_len(int q) {
    if (count_trans(q) == 0) {
        //contains data about the first state along the path from its parent to q
        //this state is for a string that is encountered once as a proper infix (if it's different from q)
        //and possibly once as a suffix, therefore pos is correct for q
        //because once set, pos is not changed after that
        //q is a suffix
        return n - Q[q].pos;
    }
    return Q[q].len;
}

//to be able to get s_link of leaves we need to store additionally parent of every node

ImplicitState follow_word(int q, int k, int pos) { //assumes word is followable
    if (k == 0) { // covers additional case for inner vertices
        return {q};
    }
    char a = word[pos];
    assert(has_trans(q,a));
    int p = delta(q, a);
    int jump = get_len(p) - Q[q].len;
    if (k < jump) {
        return {q, k, a};
    }
    return follow_word(p, k - jump, pos + jump);
}

ImplicitState ImplicitState::follow_char(char b) const {
    if (k == 0) {
        if (!has_trans(q, b)) {
            return {0};
        }
        int p = delta(q, b);
        if (get_len(p) - Q[q].len == 1) {
            return {p};
        }
        return {q, 1, b};
    }
    int p = delta(q, a);
    if (word[Q[p].pos + Q[q].len + k] != b) { 
        return {0};
    }
    if (k == get_len(p) - Q[q].len - 1) {
        return {p};
    }
    return {q, k+1, a};
}

ImplicitState ImplicitState::get_s_link() const {
    if (Q[q].len == 0) {
        if (!k) {
            return {0};
        }
        int p = delta(q, a);
        int i = Q[p].pos + 1;
        return follow_word(q, k-1, i);
    } 
    if (k == 0) {
        return {Q[q].s_link};
    }
    int l = Q[q].s_link;
    int p = delta(q, a);
    int i = Q[p].pos + Q[q].len;
    return follow_word(l, k, i);
}

string_view get_str(int q) {
    return {word + Q[q].pos, get_len(q)};
}

int qn = 1;
ImplicitState stem;

void add_char() {
    char c = word[n++];

    int prev_stem = 0;
    while (stem && !stem.follow_char(c)) {
        int inner_stem = 0;
        //creation of positions is fucked up
        if (stem.k) { // make explicit state from implicit one
            int p = delta(stem.q, stem.a);
            Q[qn] = {Q[p].pos, Q[stem.q].len + stem.k}; // assign suffix link at next iteration
            delta(stem.q, stem.a) = qn;
            char a = word[Q[qn].pos + Q[qn].len];
            delta(qn, a) = p;
            inner_stem = qn;
            if (prev_stem) {
                Q[prev_stem].s_link = inner_stem;
            }
            ++qn;
        } else {
            inner_stem = stem.q;
        }
        //create destination state
        Q[qn] = {n - Q[inner_stem].len - 1, Q[inner_stem].len + 1};
        delta(inner_stem, c) = qn;
        qn++;

        prev_stem = inner_stem;
        stem = stem.get_s_link();
    }

    //if the while did do an iteration
    //then since stem was not a leaf and had a new transition added
    //therefore the current stem has that other transition as well apart from the transition with c
    //so the k of stem decreased during every iteration until it reached 0

    if (prev_stem) { 
        //stem is inner state due to above reasoning
        Q[prev_stem].s_link = stem.q; 
        // prev_stem might be newly created, otherwise its s_link is already the same, so OK
        // if stem is null then prev_stem is empty word, so OK
    }

    if (!stem) {
        stem = {1};
        //empty word is never a leaf
    } else {
        stem = stem.follow_char(c);
        // not a leaf because proper infix
    }
}

void init() {
    qn = 1;
    n = 0;
    //create empty word
    Q[qn] = {-1, 0}; 
    stem = {qn};
    qn++;
    Q[qn] = {0, 1};
    delta(qn-1, word[n++]) = qn;
    qn++;
}

int main() {
    cin >> word;
    int n = strlen(word);
    init(); // init uses 1st letter
    for (int i=1; i<n; ++i) {
        add_char();
    }
    cout << qn-1 << " inner states: " << endl;
    for (int q=1; q<=qn; ++q) {
        cout << q << ' ';
        cout << get_str(q) << endl;
        cout << endl;
    }
}
