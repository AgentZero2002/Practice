/**
 * Manacher 算法 — 最长回文子串 O(n)
 *
 * 问题: 给定字符串 s，找出其最长回文子串。
 *
 * 朴素方法:
 *   - 暴力枚举 O(n³)
 *   - 中心扩展 O(n²)
 *
 * Manacher 算法 O(n):
 *   1. 预处理: 在字符间插入分隔符 '#'，统一处理奇偶长度
 *      "abba" → "#a#b#b#a#"
 *   2. 维护最右回文边界，利用对称性跳过已知计算
 *   3. P[i] = 以 i 为中心的回文半径
 *      原串回文长度 = P[i] (去除 #)
 *
 * 核心思想: 当我们知道一个大回文子串的对称中心 C 和右边界 R 时，
 *   对于在 C 和 R 之间的位置 i，其回文半径至少等于
 *   其关于 C 的对称位置 i' 的回文半径（但不超过 R-i）。
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

string manacher(const string& s) {
    if (s.empty()) return "";

    // 1. 预处理插入分隔符
    string t = "#";
    for (char c : s) {
        t += c;
        t += '#';
    }

    int n = t.length();
    vector<int> P(n, 0);  // P[i] = 以 i 为中心的回文半径
    int C = 0, R = 0;     // 最右回文的中心和右边界

    int maxLen = 0, maxCenter = 0;

    for (int i = 0; i < n; i++) {
        // 2. 利用对称性确定初始半径
        int i_mirror = 2 * C - i;  // i 关于 C 的对称点

        if (i < R)
            P[i] = min(R - i, P[i_mirror]);
        else
            P[i] = 0;

        // 3. 中心扩展
        while (i - P[i] - 1 >= 0 && i + P[i] + 1 < n &&
               t[i - P[i] - 1] == t[i + P[i] + 1]) {
            P[i]++;
        }

        // 4. 更新最右边界
        if (i + P[i] > R) {
            C = i;
            R = i + P[i];
        }

        // 5. 更新全局最长
        if (P[i] > maxLen) {
            maxLen = P[i];
            maxCenter = i;
        }
    }

    // 从预处理字符串恢复到原字符串的下标
    // t[maxCenter - maxLen .. maxCenter + maxLen]
    // 原字符串下标 = (t_idx - 1) / 2
    int start = (maxCenter - maxLen) / 2;
    return s.substr(start, maxLen);
}

// ==================== 验证 ====================
bool isPalindrome(const string& s) {
    int l = 0, r = s.length() - 1;
    while (l < r) {
        if (s[l] != s[r]) return false;
        l++; r--;
    }
    return true;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Manacher 最长回文子串 O(n)             " << endl;
    cout << "========================================" << endl << endl;

    vector<string> tests = {
        "babad",
        "cbbd",
        "racecar",
        "a",
        "abacdfgdcaba",
        "aaaa"
    };

    for (const string& s : tests) {
        string lps = manacher(s);
        cout << "  \"" << s << "\" → \"" << lps
             << "\" (长度 " << lps.length()
             << ", " << (isPalindrome(lps) ? "✓" : "✗") << ")" << endl;
    }

    cout << endl << "[算法关键]" << endl;
    cout << "  预处理:  在每字符间插入 '#'，回文统一处理" << endl;
    cout << "  对称性:  i 关于 C 对称点 i' = 2C - i" << endl;
    cout << "  限制:    P[i] 不能超过 R - i (信息不足)" << endl;
    cout << "  复杂度:  每个字符至多被扩展一次 → O(n)" << endl;

    return 0;
}
