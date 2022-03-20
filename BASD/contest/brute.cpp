#include <iostream>
#include <cstring>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
using namespace std;

const int MAXN = 2e5;
char word[MAXN];

int main() {
    cin >> word;
    int n = strlen(word);

    unordered_map<string, set<int>> endpos;
    map<set<int>, string> states;
    unordered_map<string, set<char>> trans;
    set<char> letters;

    for (int i=0; i<n; ++i) {
        letters.insert(word[i]);
        string curr;
        for (int j=i; j<n; ++j) {
            curr.push_back(word[j]);
            endpos[curr].insert(j);
        }
    }
    for (const auto &[str, pos] : endpos) {
        if (str.size() > states[pos].size()) {
            states[pos] = str;
        }
    }
    int qn = states.size();
    int fn = 0;
    int ans = 0;
    for (const auto &[pos, str] : states) {
        for (int p : pos) {
            if (p+1 < n) {
                trans[str].insert(word[p+1]);
            }
        }
        if (pos.find(n-1) != pos.end()) {
            ++fn;
        }
        if (str.substr(0,str.size()/2) == str.substr(str.size()/2)) {
            //cout << "counting " << str << endl;
            ++ans;
        }
    }
    
    int dn = 0;
    for (const auto &[_, t] : trans) {
        dn += t.size();
    }
    cout << qn+1              << endl;
    cout << dn+letters.size() << endl;
    cout << fn+1              << endl;
    cout << ans               << endl;
}
