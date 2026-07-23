/**
 * 状态压缩 DP (State Compression Dynamic Programming)
 *
 * 核心思想:
 *   当问题状态可以用集合表示, 且元素个数较少 (通常 n ≤ 20) 时,
 *   用整数的二进制位表示集合, 实现 O(1) 的状态转移。
 *
 * 典型特征:
 *   - n ≤ 20 (状态总数 2^n ≈ 10^6)
 *   - 每个元素只有两种状态 (选/不选, 访问/未访问)
 *   - dp[mask][...] 中 mask 是二进制压缩的集合
 *
 * 经典问题:
 *   1. TSP (旅行商问题): dp[mask][i] — 已访问 mask 集合, 最后在城市 i
 *   2. 铺砖问题: dp[i][mask] — 第 i 行状态为 mask 的方案数
 *   3. 集合覆盖 / 指派问题
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// ==================== 1. TSP 旅行商问题 ====================
// dp[mask][i] = 从 0 出发, 访问完 mask 集合中的城市, 最后停在 i 的最短路径
int tsp(const vector<vector<int>>& dist) {
    int n = dist.size();
    int N = 1 << n;
    vector<vector<int>> dp(N, vector<int>(n, INT_MAX / 2));

    dp[1][0] = 0;  // 从城市 0 出发

    for (int mask = 1; mask < N; mask++) {
        for (int u = 0; u < n; u++) {
            if (!(mask & (1 << u))) continue;       // u 必须在 mask 中
            if (dp[mask][u] == INT_MAX / 2) continue;
            for (int v = 0; v < n; v++) {
                if (mask & (1 << v)) continue;       // v 不能在 mask 中
                int nxt = mask | (1 << v);
                dp[nxt][v] = min(dp[nxt][v], dp[mask][u] + dist[u][v]);
            }
        }
    }

    // 回到起点
    int ans = INT_MAX / 2;
    for (int u = 1; u < n; u++)
        ans = min(ans, dp[N - 1][u] + dist[u][0]);
    return ans;
}

// ==================== 2. 铺砖问题 (1×2 多米诺覆盖 N×M 网格) ====================
// dp[row][mask]: 铺完前 row 行, 第 row 行状态为 mask 的方案数
// mask 的 bit j = 1 表示该位置已被 (竖砖的上半部分) 占用
int dominoCover(int N, int M) {
    int states = 1 << M;
    vector<long long> dp(states, 0);
    dp[0] = 1;

    for (int row = 0; row < N; row++) {
        vector<long long> ndp(states, 0);
        for (int mask = 0; mask < states; mask++) {
            if (dp[mask] == 0) continue;
            // 枚举当前行的横砖放置方式
            // 递归生成合法状态
            function<void(int, int, int)> dfs = [&](int col, int curMask, int nxtMask) {
                if (col == M) {
                    ndp[nxtMask] += dp[mask];
                    return;
                }
                if (curMask & (1 << col)) {
                    // 当前位置已被竖砖占, 必须留空
                    dfs(col + 1, curMask, nxtMask);
                } else {
                    // 放竖砖 (占当前行+下一行)
                    dfs(col + 1, curMask, nxtMask | (1 << col));
                    // 放横砖 (占当前行相邻两格)
                    if (col + 1 < M && !(curMask & (1 << (col + 1))))
                        dfs(col + 2, curMask, nxtMask);
                }
            };
            dfs(0, mask, 0);
        }
        dp = ndp;
    }
    return dp[0];  // 最后一行必须无竖砖伸出
}

// ==================== 3. 集合覆盖 (最小顶点覆盖) ====================
// 枚举子集的经典技巧: sub = (sub - 1) & mask
void enumerateSubsets(int mask) {
    vector<int> subsets;
    for (int sub = mask; sub; sub = (sub - 1) & mask)
        subsets.push_back(sub);
    subsets.push_back(0);  // 空集

    cout << "  mask = " << mask << " 的所有子集: ";
    for (int s : subsets) cout << s << " ";
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  状态压缩 DP                            " << endl;
    cout << "========================================" << endl << endl;

    // 1. TSP
    cout << "[TSP 旅行商问题]" << endl;
    vector<vector<int>> dist = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0}
    };
    cout << "  4 城市, 最短哈密顿回路: " << tsp(dist) << " (期望: 80)" << endl;
    cout << endl;

    // 2. 铺砖
    cout << "[铺砖问题] 3×4 网格, 1×2 多米诺" << endl;
    cout << "  覆盖方案数: " << dominoCover(3, 4) << " (期望: 11)" << endl;
    cout << endl;

    // 3. 枚举子集
    cout << "[枚举子集技巧]" << endl;
    enumerateSubsets(5);  // 5 = 101b = {0,2}
    cout << endl;

    cout << "[状压DP关键技巧]" << endl;
    cout << "  判断 i 在 mask 中: mask & (1<<i)" << endl;
    cout << "  添加 i 到 mask: mask | (1<<i)" << endl;
    cout << "  枚举子集: for(sub=mask; sub; sub=(sub-1)&mask)" << endl;

    return 0;
}
