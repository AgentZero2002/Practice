/**
 * LCP 数组 — Kasai 算法 (Longest Common Prefix)
 *
 * LCP[i] = 后缀 SA[i] 与后缀 SA[i-1] 的最长公共前缀长度
 * 即在字典序中相邻的两个后缀的公共前缀长度。
 *
 * Kasai 算法 O(n):
 *   利用关键性质: LCP[rank[i]] ≥ LCP[rank[i-1]] - 1
 *   (相邻后缀去掉首字符后, LCP 最多减少 1)
 *
 *   由此可以从前往后线性计算, 不需每次从 0 开始。
 *
 * 应用:
 *   1. 最长重复子串 = max(LCP)
 *   2. 不同子串数 = n(n+1)/2 - Σ LCP[i]
 *   3. 最长公共子串 (两串拼接): 用分隔符 # 拼接后查 SA 中
 *      分属两串且 LCP 最大的相邻后缀
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// 后缀数组 (复用)
vector<int> buildSA(const string& s) {
    int n = s.length();
    vector<int> sa(n), rank(n), tmp(n);
    for (int i = 0; i < n; i++) { sa[i] = i; rank[i] = s[i]; }
    for (int k = 1; k < n; k <<= 1) {
        auto cmp = [&](int i, int j) {
            if (rank[i] != rank[j]) return rank[i] < rank[j];
            int ri = (i + k < n) ? rank[i + k] : -1;
            int rj = (j + k < n) ? rank[j + k] : -1;
            return ri < rj;
        };
        sort(sa.begin(), sa.end(), cmp);
        tmp[sa[0]] = 0;
        for (int i = 1; i < n; i++)
            tmp[sa[i]] = tmp[sa[i-1]] + (cmp(sa[i-1], sa[i]) ? 1 : 0);
        rank.swap(tmp);
        if (rank[sa[n-1]] == n - 1) break;
    }
    return sa;
}

// Kasai 算法: O(n) 构建 LCP 数组
vector<int> buildLCP(const string& s, const vector<int>& sa) {
    int n = s.length();
    vector<int> rank(n), lcp(n - 1);

    for (int i = 0; i < n; i++) rank[sa[i]] = i;

    int h = 0;
    for (int i = 0; i < n; i++) {
        if (rank[i] == 0) continue;
        int j = sa[rank[i] - 1];
        while (i + h < n && j + h < n && s[i + h] == s[j + h])
            h++;
        lcp[rank[i] - 1] = h;
        if (h > 0) h--;
    }
    return lcp;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  LCP 数组 — Kasai 算法 O(n)              " << endl;
    cout << "========================================" << endl << endl;

    string s = "banana";
    auto sa = buildSA(s);
    auto lcp = buildLCP(s, sa);

    cout << "[字符串] \"" << s << "\"" << endl << endl;
    cout << "[SA + LCP 表]" << endl;
    cout << "  i | SA | LCP | 后缀" << endl;
    for (int i = 0; i < (int)sa.size(); i++) {
        cout << "  " << i << " | " << sa[i] << "  | "
             << (i > 0 ? to_string(lcp[i-1]) : "-")
             << "   | " << s.substr(sa[i]) << endl;
    }

    // 最长重复子串
    int maxLCP = 0, maxIdx = 0;
    for (int i = 0; i < (int)lcp.size(); i++)
        if (lcp[i] > maxLCP) { maxLCP = lcp[i]; maxIdx = i; }

    cout << endl << "[最长重复子串] \"" << s.substr(sa[maxIdx], maxLCP)
         << "\" (长度 " << maxLCP << ")" << endl;

    // 不同子串数
    long long total = (long long)s.length() * (s.length() + 1) / 2;
    for (int len : lcp) total -= len;
    cout << "[不同子串数] " << total << endl;

    return 0;
}
