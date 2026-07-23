/**
 * DP 优化入门 (DP Optimization)
 *
 * 当基础 DP 的时间复杂度过高时 (如 O(n²) → O(n log n)),
 * 需要借助数据结构或数学工具进行优化。
 *
 * 本文件介绍两种经典优化:
 *
 * 1. 单调队列优化 (Monotonic Queue Optimization):
 *    适用于转移形如 dp[i] = max/min { dp[j] + f(j,i) } 的场景,
 *    其中 j 的范围是一个滑动窗口 [i-W, i-1]。
 *    经典问题: 滑动窗口最大值、多重背包 O(nW) 优化
 *
 * 2. 斜率优化 / 凸包技巧 (Convex Hull Trick):
 *    适用于转移形如 dp[i] = min { a[j]·x[i] + b[j] } + c[i] 的场景,
 *    其中 a[j] 和 x[i] 单调时有 O(n) 的简单写法。
 *    经典问题: 任务划分、土地购买
 */

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <climits>
using namespace std;

// ==================== 1. 单调队列优化 ====================
// 例题: 给定数组 A, 选一个长度为 K 的连续子数组, 最大化子数组和
// 等价于: max sum[i]-sum[j] s.t. i-K ≤ j < i
// 用单调队列维护 j 窗口中最小 sum[j]
int maxSubarraySumK(const vector<int>& A, int K) {
    int n = A.size();
    vector<long long> sum(n + 1, 0);
    for (int i = 0; i < n; i++)
        sum[i + 1] = sum[i] + A[i];

    deque<int> dq;  // 存储下标, sum 递增
    long long ans = LLONG_MIN;

    for (int i = 0; i <= n; i++) {
        // 移除过期元素
        while (!dq.empty() && dq.front() < i - K)
            dq.pop_front();

        if (!dq.empty())
            ans = max(ans, sum[i] - sum[dq.front()]);

        // 维护递增: 把比当前大的弹出
        while (!dq.empty() && sum[dq.back()] >= sum[i])
            dq.pop_back();
        dq.push_back(i);
    }
    return ans;
}

// ==================== 2. 多重背包的单调队列优化 ====================
// 传统多重背包 O(W·Σcnt), 单调队列优化至 O(nW)
int knapsackMultipleOpt(int W, const vector<int>& weight,
                        const vector<int>& value, const vector<int>& cnt) {
    vector<int> dp(W + 1, 0);
    int n = weight.size();

    for (int i = 0; i < n; i++) {
        int w = weight[i], v = value[i], c = cnt[i];
        // 按余数分组
        for (int r = 0; r < w; r++) {
            deque<pair<int,int>> dq;  // {value, index}
            for (int k = 0; r + k * w <= W; k++) {
                int j = r + k * w;
                int val = dp[j] - k * v;
                // 移除过期
                while (!dq.empty() && dq.front().second < k - c)
                    dq.pop_front();
                // 维护递减
                while (!dq.empty() && dq.back().first <= val)
                    dq.pop_back();
                dq.push_back({val, k});
                dp[j] = dq.front().first + k * v;
            }
        }
    }
    return dp[W];
}

// ==================== 3. 凸包技巧 (Convex Hull Trick) 简化版 ====================
// dp[i] = min{ a[j] · x[i] + b[j] } + c[i], j < i
// 当 a[j] 单调递减, x[i] 单调递增时可用单调队列 O(n)
//
// 例题: 将数组 A 分成若干段, 每段代价 = (段和)² + M,
//       求最小总代价
// dp[i] = min{ dp[j] + (sum[i]-sum[j])² + M }, 0 ≤ j < i
// 展开: dp[i] = min{ -2·sum[j]·sum[i] + (dp[j]+sum[j]²) } + sum[i]² + M
//          a[j] = -2·sum[j], x[i] = sum[i], b[j] = dp[j]+sum[j]²
long long partitionCost(const vector<int>& A, long long M) {
    int n = A.size();
    vector<long long> sum(n + 1, 0);
    for (int i = 0; i < n; i++)
        sum[i + 1] = sum[i] + A[i];

    vector<long long> dp(n + 1, LLONG_MAX);
    dp[0] = 0;

    // 直线: y = m·x + b, 其中 m = a[j] = -2·sum[j], b = b[j] = dp[j]+sum[j]²
    // 用单调队列维护"下凸壳", 队首是最优决策
    struct Line {
        long long m, b;
        // 与另一条直线的交点 x 坐标
        double intersect(const Line& other) const {
            return (double)(other.b - b) / (m - other.m);
        }
    };

    deque<Line> hull;
    hull.push_back({-2 * sum[0], dp[0] + sum[0] * sum[0]});

    for (int i = 1; i <= n; i++) {
        long long x = sum[i];
        // 弹出队首 (队首不再是当前 x 的最优)
        while (hull.size() >= 2) {
            Line& l1 = hull[0];
            Line& l2 = hull[1];
            if (l1.m * x + l1.b >= l2.m * x + l2.b)
                hull.pop_front();
            else
                break;
        }

        // 计算 dp[i]
        Line& best = hull.front();
        dp[i] = best.m * x + best.b + x * x + M;

        // 插入新直线
        Line newLine = {-2 * sum[i], dp[i] + sum[i] * sum[i]};
        while (hull.size() >= 2) {
            Line& l1 = hull[hull.size() - 2];
            Line& l2 = hull.back();
            // 新线与 l1 的交点在 l1 与 l2 的交点左边 → l2 无用
            if (newLine.intersect(l1) <= l2.intersect(l1))
                hull.pop_back();
            else
                break;
        }
        hull.push_back(newLine);
    }
    return dp[n];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  DP 优化入门                            " << endl;
    cout << "========================================" << endl << endl;

    // 1. 单调队列：滑动窗口最大子数组和
    vector<int> A = {2, -1, 4, -3, 5, -2, 1};
    int K = 3;
    cout << "[单调队列优化] 长度≤3的最大子数组和" << endl;
    cout << "  " << maxSubarraySumK(A, K)
         << " (期望: 6, 子数组 [4,-3,5]?)" << endl;
    cout << endl;

    // 2. 多重背包 O(nW)
    vector<int> w2 = {3, 4}, v2 = {5, 6}, c2 = {2, 3};
    cout << "[多重背包单调队列] W=10" << endl;
    cout << "  常规: " << knapsackMultipleOpt(10, w2, v2, c2) << endl;
    cout << endl;

    // 3. CHT 分段
    vector<int> A2 = {1, 2, 3, 4};
    long long M2 = 10;
    cout << "[凸包优化] 分段代价 sum²+M, M=" << M2 << endl;
    cout << "  最小代价: " << partitionCost(A2, M2) << endl;
    cout << endl;

    cout << "[优化方法选择]" << endl;
    cout << "  滑动窗口 + 线性代价 → 单调队列" << endl;
    cout << "  dp[j]+f(j)·g(i) → 凸包技巧 (CHT)" << endl;
    cout << "  不等式约束 → 四边形不等式 (决策单调性)" << endl;

    return 0;
}
