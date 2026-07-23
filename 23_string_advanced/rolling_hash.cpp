/**
 * 滚动哈希 (Rolling Hash / Rabin-Karp)
 *
 * 对应教材 第11章：串 — 扩展实现
 *
 * 滚动哈希是一种将字符串映射为数值的技术：
 *   hash(s) = Σ s[i] × p^i  (mod M)
 *
 * 其中 p 为基数（如 31, 131），M 为一个大素数或 2^64。
 *
 * 核心性质: 已知子串 s[l..r] 的哈希前缀，O(1) 计算:
 *   hash(l,r) = (prefix[r+1] - prefix[l] × p^{r-l+1}) mod M
 *
 * 应用:
 *   1. O(n) 字符串匹配 (Rabin-Karp)
 *   2. 最长公共子串 (LCP 二分)
 *   3. 回文判定 (正反哈希)
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

using ull = unsigned long long;

class RollingHash {
private:
    string s;
    int n;
    static const ull BASE = 131;
    vector<ull> prefix;   // 前缀哈希
    vector<ull> powBase;  // p^i

public:
    RollingHash(const string& str) : s(str), n(str.length()) {
        prefix.resize(n + 1, 0);
        powBase.resize(n + 1, 1);

        for (int i = 0; i < n; i++) {
            prefix[i + 1] = prefix[i] * BASE + s[i];
            powBase[i + 1] = powBase[i] * BASE;
        }
    }

    // 子串 s[l..r] 的哈希值 (0-indexed)
    ull hash(int l, int r) {
        return prefix[r + 1] - prefix[l] * powBase[r - l + 1];
    }

    // 判断两段是否相等
    bool isEqual(int l1, int r1, int l2, int r2) {
        if (r1 - l1 != r2 - l2) return false;
        return hash(l1, r1) == hash(l2, r2);
    }
};

// ==================== Rabin-Karp 字符串匹配 ====================
// 返回模式串 P 在文本 T 中首次出现的位置
int rabinKarp(const string& text, const string& pattern) {
    int n = text.length(), m = pattern.length();
    if (m == 0) return 0;
    if (n < m) return -1;

    RollingHash textHash(text);
    RollingHash patHash(pattern);
    ull pHash = patHash.hash(0, m - 1);

    for (int i = 0; i <= n - m; i++) {
        if (textHash.hash(i, i + m - 1) == pHash) {
            // 二次验证（防哈希碰撞）
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false; break;
                }
            }
            if (match) return i;
        }
    }

    return -1;
}

// ==================== 子串判定的应用 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  滚动哈希 (Rolling Hash)                " << endl;
    cout << "========================================" << endl << endl;

    // 1. 基本哈希
    string s = "abacabad";
    RollingHash rh(s);
    cout << "[字符串哈希] s = \"" << s << "\"" << endl;

    // 找所有 "aba" 出现的位置
    cout << "  查询 'aba' 出现位置: ";
    RollingHash pat("aba");
    ull pHash = pat.hash(0, 2);
    for (int i = 0; i <= (int)s.length() - 3; i++) {
        if (rh.hash(i, i + 2) == pHash)
            cout << i << " ";
    }
    cout << "(期望: 0, 4)" << endl << endl;

    // 2. 最长公共前缀 (LCP) 二分
    cout << "[最长公共前缀 LCP]" << endl;
    string s1 = "ababc", s2 = "ababd";
    RollingHash rh1(s1), rh2(s2);

    cout << "  s1 = \"" << s1 << "\", s2 = \"" << s2 << "\"" << endl;
    int lcp = 0;
    for (int len = 1; len <= min(s1.length(), s2.length()); len++) {
        if (rh1.hash(0, len - 1) == rh2.hash(0, len - 1))
            lcp = len;
        else
            break;
    }
    cout << "  LCP 长度 = " << lcp << " (\"" << s1.substr(0, lcp) << "\")" << endl << endl;

    // 3. Rabin-Karp 匹配
    cout << "[Rabin-Karp 字符串匹配]" << endl;
    string T = "ababcabcabababd";
    string P = "ababd";
    int pos = rabinKarp(T, P);
    cout << "  T = \"" << T << "\"" << endl;
    cout << "  P = \"" << P << "\"" << endl;
    cout << "  首次匹配位置: " << pos << " (期望: 11)" << endl;
    cout << "  注: 用了二次验证防哈希碰撞" << endl;

    cout << endl << "[哈希碰撞]" << endl;
    cout << "  使用 2^64 自然溢出: 单哈希，碰撞概率 ≈ 1/2^64" << endl;
    cout << "  双哈希 (两个不同模数): 碰撞概率 ≈ 1/2^128，几乎不可能碰撞" << endl;

    return 0;
}
