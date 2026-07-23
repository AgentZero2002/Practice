/**
 * SAM 应用 — 最长公共子串 & 出现次数
 *
 * 后缀自动机的两个核心应用:
 *
 * 1. 最长公共子串 (LCS):
 *    对串 S 构建 SAM, 在 SAM 上运行串 T:
 *      - 匹配时: curLen++, cur = next[c]
 *      - 失配时: 沿 link 跳转, curLen = len[link]
 *      - 最大 curLen 即为答案
 *    复杂度: O(|S| + |T|)
 *
 * 2. 每个子串的出现次数:
 *    在构建时记录每次 extend 创建的结点 (cur) 的出现
 *    (每个前缀对应的状态的出现次数 = 1)
 *    在 Parent Tree (link树) 上做 DFS, 子结点的出现次数
 *    累加到父结点 (因为父结点代表更多出现位置的等价类)
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

struct SAMState {
    int len, link;
    map<char, int> next;
    int occur;  // 出现次数
    SAMState() : len(0), link(-1), occur(0) {}
};

class SuffixAutomaton {
public:
    vector<SAMState> st;
    int last;

    SuffixAutomaton() {
        st.push_back(SAMState());
        last = 0;
    }

    void extend(char c) {
        int cur = st.size();
        st.push_back(SAMState());
        st[cur].len = st[last].len + 1;
        st[cur].occur = 1;  // 标记为前缀状态

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
                st[clone].occur = 0;  // clone 状态不是前缀

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

    // 计算出现次数 (Parent Tree 后序遍历)
    void calcOccurrences() {
        vector<vector<int>> tree(st.size());
        for (int i = 1; i < (int)st.size(); i++)
            tree[st[i].link].push_back(i);

        function<void(int)> dfs = [&](int u) {
            for (int v : tree[u]) {
                dfs(v);
                st[u].occur += st[v].occur;
            }
        };
        dfs(0);
    }

    // 最长公共子串 (LCS)
    int longestCommonSubstring(const string& T) {
        int cur = 0, curLen = 0, maxLen = 0;
        for (char c : T) {
            while (cur != 0 && !st[cur].next.count(c)) {
                cur = st[cur].link;
                curLen = st[cur].len;
            }
            if (st[cur].next.count(c)) {
                cur = st[cur].next[c];
                curLen++;
            }
            maxLen = max(maxLen, curLen);
        }
        return maxLen;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  SAM 应用 — LCS & 出现次数               " << endl;
    cout << "========================================" << endl << endl;

    SuffixAutomaton sam;
    string S = "ababa";
    sam.build(S);
    sam.calcOccurrences();

    // 1. 出现次数
    cout << "[子串出现次数] S=\"" << S << "\"" << endl;
    cout << "  状态数: " << sam.st.size() << endl;
    cout << "  在 Parent Tree 中: 叶状态=前缀, 内部=多出现位置" << endl;

    // 2. LCS
    string T = "baba";
    int lcs = sam.longestCommonSubstring(T);
    cout << endl << "[最长公共子串] S=\"" << S << "\", T=\"" << T << "\"" << endl;
    cout << "  LCS 长度: " << lcs << " (\"aba\" 或 \"bab\")" << endl;
    cout << "  算法: 在 S 的 SAM 上运行 T, 失配时沿 link 跳" << endl;

    cout << endl << "[SAM vs 后缀数组]" << endl;
    cout << "  SAM: O(n) 构建, 在线, 适合动态添加" << endl;
    cout << "  SA:  O(n log n) 构建, 数组实现更轻量" << endl;
    cout << "  大多数问题两者都可以解" << endl;

    return 0;
}
