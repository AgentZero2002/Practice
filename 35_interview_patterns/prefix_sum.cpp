/**
 * 前缀和 + 哈希表 — 面试高频优化模式
 *
 * 将 "记录之前某段区域的信息" 简化为 O(1) 查询。
 * 与哈希表结合后，能将很多 O(n²) 问题降为 O(n)。
 *
 * 核心公式:
 *   sum[i..j] = prefix[j+1] - prefix[i]
 *   若要求 sum[i..j]=k，则 prefix[j+1]-prefix[i]=k
 *   即 prefix[i] = prefix[j+1] - k
 *
 * 经典题:
 *   1. 和为K的子数组     (LeetCode 560) ★最高频
 *   2. 连续子数组和       (LeetCode 523) — 同余定理
 *   3. 区域和检索 - 不可变 (LeetCode 303) — 基础
 */

#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

// ==================== 1. 和为 K 的子数组个数 ★ ====================
// 前缀和 + 哈希: O(n)
int subarraySum(const vector<int>& nums, int k) {
    unordered_map<int, int> prefixCount;  // prefix → 出现次数
    prefixCount[0] = 1;  // 空前缀

    int prefix = 0, result = 0;
    for (int x : nums) {
        prefix += x;
        // 查找之前有几个前缀 = prefix - k
        if (prefixCount.count(prefix - k))
            result += prefixCount[prefix - k];
        prefixCount[prefix]++;
    }
    return result;
}

// ==================== 2. 连续子数组和 (同余定理) ====================
// 判断是否存在长度 ≥2 的连续子数组和为 k 的倍数
bool checkSubarraySum(const vector<int>& nums, int k) {
    unordered_map<int, int> modIndex;  // remainder → 最早出现下标
    modIndex[0] = -1;

    int prefix = 0;
    for (int i = 0; i < (int)nums.size(); i++) {
        prefix += nums[i];
        int remainder = prefix % k;

        if (modIndex.count(remainder)) {
            if (i - modIndex[remainder] >= 2)
                return true;
        } else {
            modIndex[remainder] = i;
        }
    }
    return false;
}

// ==================== 3. 二维区域和 (前缀和矩阵) ====================
class NumMatrix {
    vector<vector<int>> prefix;
public:
    NumMatrix(const vector<vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        prefix.assign(m + 1, vector<int>(n + 1, 0));

        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                prefix[i + 1][j + 1] = prefix[i][j + 1] +
                    prefix[i + 1][j] - prefix[i][j] + matrix[i][j];
    }

    int sumRegion(int r1, int c1, int r2, int c2) {
        return prefix[r2 + 1][c2 + 1] - prefix[r1][c2 + 1]
             - prefix[r2 + 1][c1] + prefix[r1][c1];
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  前缀和 + 哈希优化                       " << endl;
    cout << "========================================" << endl << endl;

    // 1. 和为K的子数组
    vector<int> nums = {1, 1, 1};
    int k = 2;
    cout << "[和为K的子数组]" << endl;
    cout << "  nums=[1,1,1], k=2 → "
         << subarraySum(nums, k) << " (期望: 2)" << endl;
    cout << "  思路: prefix[j]=k+prefix[i] → 哈希存 prefix 的出现次数" << endl << endl;

    // 2. 同余
    vector<int> nums2 = {23, 2, 4, 6, 7};
    cout << "[连续子数组和 — 同余定理]" << endl;
    cout << "  nums=[23,2,4,6,7], k=6 → "
         << (checkSubarraySum(nums2, 6) ? "True" : "False")
         << " (期望: True, [2,4] sum=6)" << endl << endl;

    // 3. 二维
    vector<vector<int>> mat = {{3,0,1,4}, {5,6,3,2}, {1,2,0,1}, {4,1,0,1}};
    NumMatrix nm(mat);
    cout << "[二维区域和检索]" << endl;
    cout << "  (0,0)→(2,3) = " << nm.sumRegion(0, 0, 2, 3) << " (期望: 27)" << endl;

    cout << endl << "[前缀和本质] 用空间换时间, 将区间查询从 O(n) 降到 O(1)" << endl;
    return 0;
}
