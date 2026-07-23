/**
 * 线性 DP 模式 (Linear DP Patterns)
 *
 * 线性 DP 是最基础的 DP 形式, 状态只依赖于前几个位置。
 * 这是 DP 入门的系统化整理。
 *
 * 核心模式:
 *
 * 1. 单序列线性 DP:
 *    dp[i] = 以位置 i 结尾的最优解
 *    经典: LIS, 最大子段和, 爬楼梯
 *
 * 2. 双序列线性 DP (对齐):
 *    dp[i][j] = 序列1前i个 与 序列2前j个 的最优解
 *    经典: LCS, 编辑距离
 *
 * 3. 状态机 DP:
 *    dp[i][state] = 位置 i 处于状态 state 的最优解
 *    经典: 股票买卖, 打家劫舍
 *
 * 时间复杂度通常是 O(n) 或 O(n²)
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// ==================== 1. LIS O(n²) + O(n log n) ====================
int lengthOfLIS(const vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 1);
    int ans = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++)
            if (nums[j] < nums[i])
                dp[i] = max(dp[i], dp[j] + 1);
        ans = max(ans, dp[i]);
    }
    return ans;
}

// LIS O(n log n) — 耐心排序 (Patience Sorting)
int lengthOfLIS_Fast(const vector<int>& nums) {
    vector<int> tails;  // tails[i] = 长度i+1的LIS的最小尾部
    for (int x : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;
    }
    return tails.size();
}

// ==================== 2. LCS — 最长公共子序列 ====================
int longestCommonSubsequence(const string& s1, const string& s2) {
    int n = s1.length(), m = s2.length();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++) {
            if (s1[i-1] == s2[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    return dp[n][m];
}

// ==================== 3. 编辑距离 (Edit Distance / Levenshtein) ====================
int editDistance(const string& word1, const string& word2) {
    int n = word1.length(), m = word2.length();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
    for (int i = 0; i <= n; i++) dp[i][0] = i;  // 删除
    for (int j = 0; j <= m; j++) dp[0][j] = j;  // 插入

    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++) {
            if (word1[i-1] == word2[j-1])
                dp[i][j] = dp[i-1][j-1];
            else
                dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        }
    return dp[n][m];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  线性 DP 基础模式                        " << endl;
    cout << "========================================" << endl << endl;

    // LIS
    vector<int> nums = {10, 9, 2, 5, 3, 7, 101, 18};
    cout << "[LIS] " << lengthOfLIS(nums)
         << " (O(n²)), " << lengthOfLIS_Fast(nums)
         << " (O(n log n)) — 期望: 4" << endl;

    // LCS
    cout << "[LCS] \"abcde\" vs \"ace\": "
         << longestCommonSubsequence("abcde", "ace")
         << " — 期望: 3 (\"ace\")" << endl;

    // Edit Distance
    cout << "[编辑距离] \"horse\" → \"ros\": "
         << editDistance("horse", "ros")
         << " — 期望: 3" << endl;

    cout << endl << "[线性DP总结]" << endl;
    cout << "  1D: dp[i] 依赖 dp[j] (j < i)" << endl;
    cout << "  2D: dp[i][j] 依赖 dp[i-1][j], dp[i][j-1], dp[i-1][j-1]" << endl;
    cout << "  状态机: dp[i][k] 依不同状态有不同转移" << endl;

    return 0;
}
