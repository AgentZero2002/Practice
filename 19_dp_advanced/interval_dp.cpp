/**
 * 区间 DP (Interval Dynamic Programming)
 *
 * 核心思想:
 *   状态由区间 [l, r] 定义, 通过枚举分割点 k 将大区间
 *   分解为两个(或多个)小区间, 按区间长度递增顺序计算。
 *
 * 标准转移:
 *   dp[l][r] = min/max { dp[l][k] + dp[k+1][r] + cost(l,k,r) }
 *   其中 l ≤ k < r
 *
 * 实现要点:
 *   1. 按区间长度 len = 2 → n 递增枚举
 *   2. 枚举左端点 l, 右端点 r = l + len - 1
 *   3. 枚举分割点 k ∈ [l, r)
 *
 * 经典问题:
 *   1. 石子合并 (区间求和DP)
 *   2. 矩阵链乘 (最优加括号)
 *   3. 回文分割 / 最长回文子序列
 *
 * 时间复杂度: O(n³), 空间: O(n²)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// ==================== 1. 石子合并 (Stone Merge) ====================
// n 堆石子排成一排, 每次合并相邻两堆, 代价为两堆石子数之和
// 求合并成一堆的最小代价
int stoneMerge(const vector<int>& stones) {
    int n = stones.size();
    vector<int> prefix(n + 1, 0);
    for (int i = 0; i < n; i++)
        prefix[i + 1] = prefix[i] + stones[i];

    auto sum = [&](int l, int r) {
        return prefix[r + 1] - prefix[l];
    };

    vector<vector<int>> dp(n, vector<int>(n, INT_MAX / 2));

    // 长度为 1 时, 不需要合并, 代价 = 0
    for (int i = 0; i < n; i++) dp[i][i] = 0;

    for (int len = 2; len <= n; len++) {
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1;
            for (int k = l; k < r; k++) {
                dp[l][r] = min(dp[l][r],
                    dp[l][k] + dp[k + 1][r] + sum(l, r));
            }
        }
    }
    return dp[0][n - 1];
}

// ==================== 2. 矩阵链乘 (Matrix Chain Multiplication) ====================
// dims = [p0, p1, ..., pn], 矩阵 Ai 为 pi-1 × pi
// 求计算 A1×A2×...×An 的最小标量乘法次数
int matrixChain(const vector<int>& dims) {
    int n = dims.size() - 1;  // 矩阵数量
    vector<vector<int>> dp(n, vector<int>(n, INT_MAX / 2));

    for (int i = 0; i < n; i++) dp[i][i] = 0;

    for (int len = 2; len <= n; len++) {
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1;
            for (int k = l; k < r; k++) {
                int cost = dims[l] * dims[k + 1] * dims[r + 1];
                dp[l][r] = min(dp[l][r],
                    dp[l][k] + dp[k + 1][r] + cost);
            }
        }
    }
    return dp[0][n - 1];
}

// ==================== 3. 最长回文子序列 (Longest Palindromic Subsequence) ====================
// 区间 DP 的二维形式: dp[l][r] = s[l..r] 中最长回文子序列长度
int longestPalindromeSubseq(const string& s) {
    int n = s.length();
    vector<vector<int>> dp(n, vector<int>(n, 0));

    for (int i = 0; i < n; i++) dp[i][i] = 1;

    for (int len = 2; len <= n; len++) {
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1;
            if (s[l] == s[r])
                dp[l][r] = dp[l + 1][r - 1] + 2;
            else
                dp[l][r] = max(dp[l + 1][r], dp[l][r - 1]);
        }
    }
    return dp[0][n - 1];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  区间 DP                                " << endl;
    cout << "========================================" << endl << endl;

    // 1. 石子合并
    vector<int> stones = {3, 4, 5, 2};
    cout << "[石子合并]" << endl;
    cout << "  石子: 3 4 5 2" << endl;
    cout << "  最小合并代价: " << stoneMerge(stones)
         << " (期望: 14+14=28? 实际看DP)" << endl;
    cout << endl;

    // 2. 矩阵链乘
    vector<int> dims = {10, 30, 5, 60};  // A1:10×30, A2:30×5, A3:5×60
    cout << "[矩阵链乘]" << endl;
    cout << "  维度: 10×30, 30×5, 5×60" << endl;
    cout << "  最小乘法次数: " << matrixChain(dims)
         << " (期望: 4500)" << endl;
    cout << endl;

    // 3. 最长回文子序列
    string s = "bbbab";
    cout << "[最长回文子序列]" << endl;
    cout << "  字符串: \"" << s << "\"" << endl;
    cout << "  最长回文子序列长度: " << longestPalindromeSubseq(s)
         << " (期望: 4, \"bbbb\")" << endl;
    cout << endl;

    cout << "[区间 DP 模板]" << endl;
    cout << "  for(len = 2..n)" << endl;
    cout << "    for(l = 0; l+len-1 < n; l++)" << endl;
    cout << "      r = l+len-1" << endl;
    cout << "      for(k = l; k < r; k++)" << endl;
    cout << "        dp[l][r] = opt(dp[l][k] + dp[k+1][r] + cost)" << endl;

    return 0;
}
