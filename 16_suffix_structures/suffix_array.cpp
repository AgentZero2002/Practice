/**
 * 后缀数组 — 倍增法 (Suffix Array via Doubling)
 *
 * 后缀数组 (SA) 是字符串处理中最强大的数据结构之一。
 * SA[i] = 字典序第 i 小的后缀的起始位置 (0-indexed)。
 *
 * 倍增法 (Prefix Doubling):
 *   1. 按第一个字符排序 (rank)
 *   2. 按前 2^k 个字符排序 (用 rank[i] 和 rank[i+2^(k-1)] 作为双关键字)
 *   3. 直到排序唯一 (所有 rank 互异)
 *
 * 每次排序: O(n log n) via std::sort
 * 总复杂度: O(n log² n) — 朴实实现
 * 最优: SA-IS 算法 O(n)
 *
 * 应用:
 *   - 最长公共前缀 (LCP Array via Kasai 算法)
 *   - 最长重复子串
 *   - 不同子串的数量: n(n+1)/2 - Σ LCP[i]
 *   - 字符串匹配: 二分查找 O(m log n)
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

vector<int> buildSA(const string& s) {
    int n = s.length();
    vector<int> sa(n), rank(n), tmp(n);

    // 初始排序: 按第一个字符
    for (int i = 0; i < n; i++) {
        sa[i] = i;
        rank[i] = s[i];
    }

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

        if (rank[sa[n-1]] == n - 1) break;  // 已经全部分离
    }
    return sa;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  后缀数组 — 倍增法 (Suffix Array)         " << endl;
    cout << "========================================" << endl << endl;

    string s = "banana";
    auto sa = buildSA(s);

    cout << "[字符串] \"" << s << "\"" << endl;
    cout << endl << "[后缀数组]" << endl;
    cout << "  i | SA[i] | 后缀" << endl;
    for (int i = 0; i < (int)sa.size(); i++) {
        cout << "  " << i << " |  " << sa[i] << "   | "
             << s.substr(sa[i]) << endl;
    }
    // 期望: "ana", "anana", "banana", "na", "nana"

    cout << endl << "[应用]" << endl;
    cout << "  不同子串数 = n(n+1)/2 - Σ LCP[i]" << endl;
    cout << "  最长重复子串 = max LCP[i]" << endl;
    cout << "  模式匹配: 在 SA 上二分查找 O(m log n)" << endl;

    return 0;
}
