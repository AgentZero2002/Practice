/**
 * 最长递增子序列 (LIS — Longest Increasing Subsequence)
 *
 * 经典题: LeetCode 300
 *
 * 方法1: DP O(n²)
 *   dp[i] = 以 A[i] 结尾的最长 LIS 长度
 *   dp[i] = max{ dp[j] + 1 | j < i and A[j] < A[i] }
 *
 * 方法2: 贪心 + 二分 O(n log n)
 *   维护 tails 数组，tails[k] = 长度 k+1 的 LIS 的最小末尾
 *   依次处理每个元素，用二分查找决定扩展或替换
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ==================== DP O(n²) ====================
int lis_dp(const vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;

    vector<int> dp(n, 1);
    int maxLen = 1;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i])
                dp[i] = max(dp[i], dp[j] + 1);
        }
        maxLen = max(maxLen, dp[i]);
    }

    return maxLen;
}

// ==================== 贪心 + 二分 O(n log n) ====================
// tails[k] = 长度为 k+1 的递增子序列的最小可能结尾元素
int lis_optimized(const vector<int>& nums) {
    vector<int> tails;

    for (int x : nums) {
        // 在 tails 中二分查找第一个 ≥ x 的位置
        auto it = lower_bound(tails.begin(), tails.end(), x);

        if (it == tails.end()) {
            tails.push_back(x);  // x 比所有 tail 都大 → 扩展 LIS
        } else {
            *it = x;  // 替换为更小的结尾（保持贪心性质）
        }
    }

    return tails.size();
}

// ==================== 同时返回 LIS 序列 ====================
vector<int> lis_sequence(const vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return {};

    vector<int> dp(n, 1), prev(n, -1);
    int maxIdx = 0;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i] && dp[j] + 1 > dp[i]) {
                dp[i] = dp[j] + 1;
                prev[i] = j;
            }
        }
        if (dp[i] > dp[maxIdx]) maxIdx = i;
    }

    // 回溯
    vector<int> seq;
    for (int i = maxIdx; i != -1; i = prev[i])
        seq.push_back(nums[i]);
    reverse(seq.begin(), seq.end());
    return seq;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  最长递增子序列 (LIS)                   " << endl;
    cout << "========================================" << endl << endl;

    vector<int> nums = {10, 9, 2, 5, 3, 7, 101, 18};
    cout << "序列: ";
    for (int x : nums) cout << x << " ";

    cout << endl << endl;
    cout << "  DP O(n²):     LIS 长度 = " << lis_dp(nums) << endl;
    cout << "  贪心+二分 O(n log n): LIS 长度 = " << lis_optimized(nums) << endl;

    auto seq = lis_sequence(nums);
    cout << "  LIS 序列: ";
    for (int x : seq) cout << x << " ";
    cout << "(期望: 2 3 7 101 或 2 5 7 101)" << endl;

    cout << endl << "[复杂度]" << endl;
    cout << "  O(n²): 标准DP，可回溯具体序列" << endl;
    cout << "  O(n log n): 贪心 + 二分，仅给出长度" << endl;
    cout << "  拓展: 二维LIS → 排序一维 + 第二维LIS" << endl;

    return 0;
}
