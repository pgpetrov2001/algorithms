#include <vector>

template<typename T1, typename T2, class DS, T2(*combine)(T2,T2)>
class SemiDynamizer {
	std::vector<T1> L[32];
	DS S[32];
public:
	SemiDynamizer() {
		L[0].reserve(1);
	}
		
	void insert(const T1 &x) {
		int i = 0;
		while (!L[i].empty()) {
			++i;
		}
		if (L[i].capacity() == 0) {
			L[i].reserve(1<<i);
		}
		for (int j=0; j<i; ++j) {
			if (L[j].empty()) continue;
			L[i].insert( L[i].end(), L[j].begin(), L[j].end() );
			L[j].clear();
		}
		L[i].push_back(x);
		S[i].init(L[i]);
	}

	T2 query(T2 x) const {
		bool have_ans = false;
		T2 ans;
		for (int i=0; i<32; ++i) {
			if (L[i].empty()) {
				continue;
			}
			T2 result = S[i].query(x);
			if (!have_ans) {
				ans = result;
				have_ans = true;
			} else {
				ans = combine(ans, result);
			}
		}
		return ans;
	}
};
