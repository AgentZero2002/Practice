/**
 * 背包问题变体 (Knapsack Variants)
 *
 * 基于崔添翼《背包九讲》的核心变体。
 *
 * 基础分类:
 *   1. 01 背包:      每件物品最多选 1 次
 *      dp[j] = max(dp[j], dp[j - w] + v)  (j 倒序)
 *
 *   2. 完全背包:     每件物品可选无限次
 *      dp[j] = max(dp[j], dp[j - w] + v)  (j 正序)
 *
 *   3. 多重背包:     第 i 件物品最多选 cnt[i] 次
 *      二进制拆分优化: 将 cnt 拆分为 1,2,4,...,余数 后做 01 背包
 *
 *   4. 分组背包:     同一组内物品最多选 1 件
 *      for 组 for 容量倒序 for 组内物品
 *
 *   5. 二维费用:     每件物品有两个维度成本 (重量+体积)
 *      dp[j][k] = max(dp[j][k], dp[j-w][k-c] + v)
 *
 * 时间复杂度:
 *   01/完全: O(nW)
 *   多重(二进制): O(W · Σ log cnt[i])
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ==================== 1. 01 背包 ====================
int knapsack01(int W, const vector<int>& weight, const vector<int>& value) {
    vector<int> dp(W + 1, 0);
    int n = weight.size();
    for (int i = 0; i < n; i++)
        for (int j = W; j >= weight[i]; j--)  // 倒序!
            dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);
    return dp[W];
}

// ==================== 2. 完全背包 ====================
int knapsackComplete(int W, const vector<int>& weight, const vector<int>& value) {
    vector<int> dp(W + 1, 0);
    int n = weight.size();
    for (int i = 0; i < n; i++)
        for (int j = weight[i]; j <= W; j++)  // 正序!
            dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);
    return dp[W];
}

// ==================== 3. 多重背包 (二进制拆分) ====================
int knapsackMultiple(int W, const vector<int>& weight,
                     const vector<int>& value, const vector<int>& cnt) {
    vector<int> dp(W + 1, 0);
    int n = weight.size();

    // 二进制拆分
    struct Item { int w, v; };
    vector<Item> items;
    for (int i = 0; i < n; i++) {
        int k = 1, remaining = cnt[i];
        while (k <= remaining) {
            items.push_back({weight[i] * k, value[i] * k});
            remaining -= k;
            k <<= 1;
        }
        if (remaining > 0)
            items.push_back({weight[i] * remaining, value[i] * remaining});
    }

    // 对拆分后的物品做 01 背包
    for (auto& item : items)
        for (int j = W; j >= item.w; j--)
            dp[j] = max(dp[j], dp[j - item.w] + item.v);

    return dp[W];
}

// ==================== 4. 分组背包 ====================
// groups[g] = {{w,v}, {w,v}, ...}, 每组最多选 1 个
int knapsackGroup(int W, const vector<vector<pair<int,int>>>& groups) {
    vector<int> dp(W + 1, 0);
    for (auto& group : groups) {
        for (int j = W; j >= 0; j--) {       // 容量倒序
            for (auto& [w, v] : group) {      // 组内枚举
                if (j >= w)
                    dp[j] = max(dp[j], dp[j - w] + v);
            }
        }
    }
    return dp[W];
}

// ==================== 5. 二维费用背包 ====================
int knapsack2D(int W, int C, const vector<int>& weight,
               const vector<int>& cost, const vector<int>& value) {
    vector<vector<int>> dp(W + 1, vector<int>(C + 1, 0));
    int n = weight.size();
    for (int i = 0; i < n; i++)
        for (int j = W; j >= weight[i]; j--)
            for (int k = C; k >= cost[i]; k--)
                dp[j][k] = max(dp[j][k], dp[j - weight[i]][k - cost[i]] + value[i]);
    return dp[W][C];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  背包问题变体 (Knapsack Variants)        " << endl;
    cout << "========================================" << endl << endl;

    int W = 10;
    vector<int> w = {2, 3, 4, 5};
    vector<int> v = {3, 4, 5, 8};

    // 01 背包
    cout << "[01 背包] W=" << W << endl;
    cout << "  最大价值: " << knapsack01(W, w, v)
         << " (期望: 13, 选 {3,4}→{2,3,5}重量=10价值=15? 待验证)" << endl;
    cout << endl;

    // 完全背包
    cout << "[完全背包] W=" << W << endl;
    cout << "  最大价值: " << knapsackComplete(W, w, v) << endl;
    cout << endl;

    // 多重背包
    vector<int> cnt = {2, 1, 2, 1};
    cout << "[多重背包] 数量限制: 2,1,2,1" << endl;
    cout << "  最大价值: " << knapsackMultiple(W, w, v, cnt) << endl;
    cout << endl;

    // 分组背包
    vector<vector<pair<int,int>>> groups = {
        {{2,3}, {3,4}},        // 组0: 二选一
        {{4,5}, {5,8}},        // 组1: 二选一
        {{3,6}}                // 组2: 必选此
    };
    cout << "[分组背包] 3组, 每组最多选1件" << endl;
    cout << "  最大价值: " << knapsackGroup(W, groups) << endl;
    cout << endl;

    cout << "[核心区分]" << endl;
    cout << "  01背包: j 倒序遍历容量" << endl;
    cout << "  完全:   j 正序遍历容量" << endl;
    cout << "  多重:   二进制拆分 + 01背包" << endl;
    cout << "  分组:   for容量 for物品 (顺序不能反)" << endl;
    cout << "  二维:   多加一维 for 循环" << endl;

    return 0;
}
