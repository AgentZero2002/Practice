/**
 * KMP 自动机 (KMP Automaton / DFA for Pattern Matching)
 *
 * 将 KMP 的 next 表扩展为一个完整的 DFA (确定性有限自动机)，
 * 对每个状态和每个可能输入字符，预计算下一状态。
 *
 * 结构:
 *   automaton[state][char] = 在 state 状态下读入 char 后的下一状态
 *
 * 构建 O(m·|Σ|):
 *   - automaton[0][P[0]] = 1
 *   - automaton[i][c] = automaton[next[i]][c]   (失配)
 *   - automaton[i][P[i]] = i+1                  (匹配)
 *
 * 匹配 O(n): 无需回溯，直接从 automaton[curState][T[i]] 转移
 *
 * 经典应用:
 *   - 在流式数据中做多字符集匹配
 *   - 构建"包含/不包含某子串"的 DAG 状态数 (数位DP)
 */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int ALPH = 26;

// 构建 KMP 自动机
vector<vector<int>> buildAutomaton(const string& pattern) {
    int m = pattern.length();
    vector<vector<int>> aut(m + 1, vector<int>(ALPH, 0));

    // 先构建 next 表
    vector<int> next(m, 0);
    next[0] = -1;
    for (int j = 0, t = -1; j < m - 1; ) {
        if (t < 0 || pattern[j] == pattern[t])
            next[++j] = ++t;
        else
            t = next[t];
    }

    // 填 DFA 表
    for (int i = 0; i <= m; i++) {
        for (int c = 0; c < ALPH; c++) {
            if (i < m && pattern[i] - 'a' == c)
                aut[i][c] = i + 1;  // 匹配 → 前进
            else if (i > 0)
                aut[i][c] = aut[next[i - 1] + 1][c];  // 失配 → 跳转
            else
                aut[i][c] = 0;  // 状态 0 失配仍为 0
        }
    }

    return aut;
}

// ==================== 应用: 计算不包含 pattern 的长度为 L 的字符串数 ====================
// 这是一个经典的 DFA + DP 问题
long long countStringsWithoutPattern(int L, const string& pattern) {
    int m = pattern.length();
    auto aut = buildAutomaton(pattern);
    const long long MOD = 1000000007;

    // dp[len][state] = 长度 len, 当前在自动机状态 state 的合法字符串数
    vector<vector<long long>> dp(L + 1, vector<long long>(m + 1, 0));
    dp[0][0] = 1;

    for (int len = 0; len < L; len++) {
        for (int state = 0; state < m; state++) {  // state < m 确保没匹配完
            if (dp[len][state] == 0) continue;
            for (int c = 0; c < ALPH; c++) {
                int nextState = aut[state][c];
                if (nextState < m)  // 仍未完全匹配 pattern
                    dp[len + 1][nextState] = (dp[len + 1][nextState] + dp[len][state]) % MOD;
            }
        }
    }

    // 统计所有不包含 pattern 的字符串
    long long total = 0;
    for (int state = 0; state < m; state++)
        total = (total + dp[L][state]) % MOD;
    return total;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  KMP 自动机 (DFA for Pattern)           " << endl;
    cout << "========================================" << endl << endl;

    string pattern = "ab";
    auto aut = buildAutomaton(pattern);

    cout << "[KMP 自动机] pattern = \"" << pattern << "\"" << endl;
    cout << "  状态转移表 (部分):" << endl;
    cout << "  state\\char | a | b | c | ..." << endl;
    for (int s = 0; s <= (int)pattern.length(); s++) {
        cout << "  " << s << "           | " << aut[s]['a'-'a']
             << " | " << aut[s]['b'-'a']
             << " | " << aut[s]['c'-'a'] << " |" << endl;
    }
    cout << "  状态 " << pattern.length() << " = 吸收态 (匹配完成)" << endl;

    // 计数应用
    cout << endl << "[应用: 不包含\"ab\"的长度为3的串]" << endl;
    long long cnt = countStringsWithoutPattern(3, "ab");
    cout << "  总数: " << cnt << " (期望: 26³=17576 - 含ab的 = 17451)" << endl;

    return 0;
}
