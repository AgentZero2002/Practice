/**
 * 树形 DP (Tree Dynamic Programming)
 *
 * 核心思想:
 *   在树上做 DP, 通常以 DFS 后序遍历的方式进行,
 *   先计算所有子节点的 DP 值, 再合并到父节点。
 *
 * 典型模式:
 *   1. 树的最大独立集: dp[u][0/1] — u 不选/选时的最大权值和
 *      dp[u][0] = Σ max(dp[v][0], dp[v][1])
 *      dp[u][1] = w[u] + Σ dp[v][0]
 *
 *   2. 树的直径 (带权): 两次 DFS 或 DP
 *      dp[u] = 从 u 向下最远的距离
 *
 *   3. 树上背包: 在树上做分组背包
 *      dp[u][k] = 以 u 为根的子树中选 k 个节点的最优值
 *
 * 时间复杂度: 通常为 O(n) 或 O(n·k) (背包型)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

// ==================== 1. 树的最大独立集 (Maximum Independent Set on Tree) ====================
// 选择不相邻的顶点, 使权值和最大
pair<int, int> maxIndependentSet(int n, const vector<pair<int,int>>& edges,
                                  const vector<int>& weight) {
    vector<vector<int>> g(n);
    for (auto& e : edges) {
        g[e.first].push_back(e.second);
        g[e.second].push_back(e.first);
    }

    // dp0[u] = u 不选, dp1[u] = u 选
    vector<int> dp0(n, 0), dp1(n, 0);

    function<void(int, int)> dfs = [&](int u, int parent) {
        dp0[u] = 0;
        dp1[u] = weight[u];
        for (int v : g[u]) {
            if (v == parent) continue;
            dfs(v, u);
            dp0[u] += max(dp0[v], dp1[v]);  // u不选, v可选可不选
            dp1[u] += dp0[v];                // u选, v不能选
        }
    };

    dfs(0, -1);
    int maxVal = max(dp0[0], dp1[0]);
    bool rootSelected = (dp1[0] >= dp0[0]);
    return {maxVal, rootSelected ? 1 : 0};
}

// ==================== 2. 树的直径 (Tree Diameter via DP) ====================
// 返回直径长度和直径端点之间的一条路径 (此处只返回长度)
int treeDiameter(int n, const vector<pair<int,int>>& edges) {
    vector<vector<int>> g(n);
    for (auto& e : edges) {
        g[e.first].push_back(e.second);
        g[e.second].push_back(e.first);
    }

    int diameter = 0;

    // dp[u] = 从 u 向下走的最长路径
    function<int(int, int)> dfs = [&](int u, int parent) -> int {
        int max1 = 0, max2 = 0;  // 最长的两条路径
        for (int v : g[u]) {
            if (v == parent) continue;
            int len = dfs(v, u) + 1;  // +1 为边 (u,v)
            if (len > max1) { max2 = max1; max1 = len; }
            else if (len > max2) { max2 = len; }
        }
        diameter = max(diameter, max1 + max2);  // 经过 u 的直径
        return max1;
    };

    dfs(0, -1);
    return diameter;
}

// ==================== 3. 树上背包 (简化版) ====================
// 以 u 为根的子树中, 选 k 个节点的最小成本 (假设每节点成本=1)
// 必须选 u 才能选它的子节点
int treeKnapsack(int n, const vector<pair<int,int>>& edges, int K) {
    vector<vector<int>> g(n);
    for (auto& e : edges) {
        g[e.first].push_back(e.second);
        g[e.second].push_back(e.first);
    }

    const int INF = 1e9;
    int ans = INF;

    // dp[u][k] = u子树中选 k 个节点的最小成本
    function<vector<int>(int, int)> dfs = [&](int u, int parent) -> vector<int> {
        vector<int> dp = {0, 1};  // dp[0] 不用, dp[1]=1 (选u自己)
        for (int v : g[u]) {
            if (v == parent) continue;
            auto child = dfs(v, u);
            // 合并: 分组背包
            int szU = dp.size() - 1;
            int szV = child.size() - 1;
            vector<int> ndp(szU + szV + 1, INF);
            for (int i = 1; i <= szU; i++)          // u子树已选 i 个
                for (int j = 0; j <= szV; j++)      // v子树选 j 个
                    if (i + j <= K)
                        ndp[i + j] = min(ndp[i + j], dp[i] + child[j]);
            dp = ndp;
        }
        if ((int)dp.size() > K && dp[K] < INF)
            ans = min(ans, dp[K]);
        return dp;
    };

    dfs(0, -1);
    return ans;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  树形 DP                                " << endl;
    cout << "========================================" << endl << endl;

    // 示例树: 0-1, 0-2, 1-3, 1-4, 2-5
    vector<pair<int,int>> edges = {{0,1}, {0,2}, {1,3}, {1,4}, {2,5}};
    int n = 6;

    // 1. 最大独立集
    vector<int> w = {1, 5, 3, 2, 4, 6};  // 各节点权值
    auto [val, root] = maxIndependentSet(n, edges, w);
    cout << "[最大独立集] 最大权值和: " << val
         << " (根节点" << (root ? "选" : "不选") << ")" << endl;
    cout << endl;

    // 2. 树的直径
    int diam = treeDiameter(n, edges);
    cout << "[树的直径] " << diam << " (期望: 3, 路径: 3-1-0-2-5)" << endl;
    cout << endl;

    // 3. 树上背包
    int K = 3;
    int cost = treeKnapsack(n, edges, K);
    cout << "[树上背包] 选 " << K << " 个连通节点最小成本: " << cost
         << " (期望: 3)" << endl;
    cout << endl;

    cout << "[树形 DP 核心公式]" << endl;
    cout << "  独立集: dp0[u]=Σmax(dp0[v],dp1[v]), dp1[u]=w[u]+Σdp0[v]" << endl;
    cout << "  直径:   max(经过u的最长链, 子树直径)" << endl;
    cout << "  背包:   每个子节点是一组, 组内不同数量是不同物品" << endl;

    return 0;
}
