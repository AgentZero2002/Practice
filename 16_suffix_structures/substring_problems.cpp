/**
 * 后缀结构综合 — 两串最长公共子串 + 最长重复子串
 *
 * 综合运用后缀数组 + LCP + 后缀自动机解决经典字符串问题。
 *
 * 问题 1: 两串的 LCS (Longest Common Substring)
 *   将 S1 和 S2 用分隔符拼接: S = S1 + '#' + S2 + '$'
 *   (两个不同分隔符确保跨串后缀不会误比较)
 *   构建 SA+LCP, 找相邻且分属两串的最大 LCP
 *
 * 问题 2: 最长重复子串 (至少出现 K 次)
 *   在 SAM 的 Parent Tree 上, 出现的次数 = 子树中前缀状态数
 *   找出现次数 ≥ K 且 len 最大的状态
 *
 * 复杂度: O(n log n) (SA) 或 O(n) (SAM)
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// ==================== 求两串的 LCS (用 SAM) ====================
// (实现复用前面的 SAM, 这里展示用简洁版本)

string twoStringLCS(const string& S, const string& T) {
    // 构建 S 的 SAM (简化版, 直接内嵌)
    struct State {
        int len, link;
        int next[26];
        State() : len(0), link(-1) { fill(next, next+26, -1); }
    };

    vector<State> st(1);
    int last = 0;

    auto extend = [&](char c) {
        int ch = c - 'a';
        int cur = st.size();
        st.push_back(State());
        st[cur].len = st[last].len + 1;

        int p = last;
        while (p != -1 && st[p].next[ch] == -1) {
            st[p].next[ch] = cur;
            p = st[p].link;
        }

        if (p == -1) st[cur].link = 0;
        else {
            int q = st[p].next[ch];
            if (st[p].len + 1 == st[q].len) st[cur].link = q;
            else {
                int clone = st.size();
                st.push_back(State());
                st[clone].len = st[p].len + 1;
                copy(st[q].next, st[q].next+26, st[clone].next);
                st[clone].link = st[q].link;
                while (p != -1 && st[p].next[ch] == q) {
                    st[p].next[ch] = clone;
                    p = st[p].link;
                }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    };

    for (char c : S) extend(c);

    // 在 SAM 上运行 T
    int cur = 0, curLen = 0, maxLen = 0, endPos = 0;
    for (int i = 0; i < (int)T.length(); i++) {
        int ch = T[i] - 'a';
        while (cur != 0 && st[cur].next[ch] == -1) {
            cur = st[cur].link;
            curLen = st[cur].len;
        }
        if (st[cur].next[ch] != -1) {
            cur = st[cur].next[ch];
            curLen++;
        }
        if (curLen > maxLen) {
            maxLen = curLen;
            endPos = i;
        }
    }

    return T.substr(endPos - maxLen + 1, maxLen);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  后缀结构综合应用                        " << endl;
    cout << "========================================" << endl << endl;

    string S = "ababc", T = "babc";
    string lcs = twoStringLCS(S, T);
    cout << "[两串LCS] S=\"" << S << "\", T=\"" << T << "\"" << endl;
    cout << "  LCS = \"" << lcs << "\" (长度 " << lcs.length()
         << ")" << endl;

    cout << endl << "[后缀结构选择]" << endl;
    cout << "  后缀数组 + LCP:    实现简单, O(n log n)" << endl;
    cout << "  后缀自动机 (SAM):  在线构建, O(n)" << endl;
    cout << "  后缀树 (Ukkonen):  在线构建, O(n), 但代码复杂" << endl;
    cout << "  推荐: SAM 是最平衡的选择" << endl;

    return 0;
}
