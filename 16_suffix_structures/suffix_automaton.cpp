/**
 * 后缀自动机 (Suffix Automaton / SAM / DAWG)
 *
 * 后缀自动机是表示一个字符串所有子串的最简 DFA (有向无环词图)。
 * 它是所有后缀数据结构的终极形态, 空间 O(n), 构建 O(n)。
 *
 * 核心性质:
 *   1. SAM 的状态数 ≤ 2n-1, 转移数 ≤ 3n-4
 *   2. 每个状态代表一组 endpos 相同的子串
 *   3. 从初始状态沿任意路径到任意状态 → 对应一个子串
 *   4. link 指针构成一棵 Parent Tree (后缀链接树)
 *
 * endpos 等价类:
 *   两个子串 u, v 在 SAM 中属于同一状态 ⟺
 *   它们在原串中的所有出现结束位置集合相同
 *
 * 状态属性:
 *   len[state]: 该状态代表的最长子串长度
 *   link[state]: 后缀链接, 指向 endpos 超集的最小状态
 *                link 树中, 状态的 endpos = 子树中叶结点数
 *
 * 在线构建 (Blumer et al. 1985):
 *   逐个字符扩展, 维护 last 指针指向当前完整字符串的状态。
 *   每次扩展可能创建 1~2 个新状态。
 *
 * 经典应用:
 *   1. 不同子串数 = Σ (len[v] - len[link[v]])
 *   2. 每个子串出现次数 = 状态的 endpos 大小
 *   3. 最长公共子串 (两串): 在 SAM 上运行第二个串
 *   4. 字典序第 K 小子串
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
using namespace std;

struct SAMState {
    int len;            // 该状态的最长子串长度
    int link;           // 后缀链接
    map<char, int> next; // 转移
    SAMState() : len(0), link(-1) {}
};

class SuffixAutomaton {
public:
    vector<SAMState> st;
    int last;

    SuffixAutomaton() {
        st.push_back(SAMState());  // 状态 0 = 初始状态
        last = 0;
    }

    void extend(char c) {
        int cur = st.size();
        st.push_back(SAMState());
        st[cur].len = st[last].len + 1;

        int p = last;
        while (p != -1 && !st[p].next.count(c)) {
            st[p].next[c] = cur;
            p = st[p].link;
        }

        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = st.size();
                st.push_back(SAMState());
                st[clone].len = st[p].len + 1;
                st[clone].next = st[q].next;
                st[clone].link = st[q].link;

                while (p != -1 && st[p].next[c] == q) {
                    st[p].next[c] = clone;
                    p = st[p].link;
                }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    void build(const string& s) {
        for (char c : s) extend(c);
    }

    // 不同子串数量
    long long distinctSubstrings() const {
        long long ans = 0;
        for (size_t i = 1; i < st.size(); i++)
            ans += st[i].len - st[st[i].link].len;
        return ans;
    }

    // 判断 t 是否为 s 的子串
    bool isSubstring(const string& t) const {
        int cur = 0;
        for (char c : t) {
            auto it = st[cur].next.find(c);
            if (it == st[cur].next.end()) return false;
            cur = it->second;
        }
        return true;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  后缀自动机 (Suffix Automaton / SAM)      " << endl;
    cout << "========================================" << endl << endl;

    string s = "ababa";
    SuffixAutomaton sam;
    sam.build(s);

    cout << "[字符串] \"" << s << "\"" << endl;
    cout << "[SAM 状态数] " << sam.st.size()
         << " (≤ 2n-1 = " << (2 * (int)s.length() - 1) << ")" << endl;
    cout << "[不同子串数] " << sam.distinctSubstrings()
         << " (枚举: a,ab,aba,abab,ababa,b,ba,bab,baba = 9)" << endl;

    cout << endl << "[子串判定]" << endl;
    for (string t : {"aba", "ba", "ab", "abc", "bab"}) {
        cout << "  \"" << t << "\" → "
             << (sam.isSubstring(t) ? "是子串 ✓" : "不是 ✗") << endl;
    }

    cout << endl << "[SAM 经典应用]" << endl;
    cout << "  不同子串数: Σ (len[v] - len[link[v]])" << endl;
    cout << "  出现次数: Parent Tree 上子树叶子数" << endl;
    cout << "  最长公共子串: 在两串SAM上运行另一串" << endl;
    cout << "  第K小子串: Parent Tree + DP" << endl;

    return 0;
}
