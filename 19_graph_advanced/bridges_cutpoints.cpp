/**
 * 桥与割点 (Bridges & Articulation Points / Cut Vertices)
 *
 * 在无向连通图中:
 *   - 桥 (Bridge): 删除该边后图不连通
 *   - 割点 (Articulation Point / Cut Vertex): 删除该顶点（及关联边）后图不连通
 *
 * Tarjan 算法:
 *   单次 DFS，计算 dTime[] 和 low[]:
 *   low[v] = min(dTime[v], dTime[u] for back edges, low[child] for tree edges)
 *
 *   桥判定: 对树边 (v→u), low[u] > dTime[v] ⟺ 桥
 *   割点判定:
 *     - 若 v 是 DFS 树的根: 有 ≥2 个孩子 → 割点
 *     - 若 v 非根: 存在孩子 u 满足 low[u] ≥ dTime[v] → 割点
 *
 * 时间复杂度: O(V + E)
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class BridgesCutPoints {
private:
    int n;
    vector<vector<int>> graph;
    vector<int> dTime, low;
    vector<bool> isCut;
    vector<pair<int, int>> bridges;
    int timer;

    void dfs(int v, int parent) {
        dTime[v] = low[v] = ++timer;
        int children = 0;

        for (int u : graph[v]) {
            if (u == parent) continue;  // 跳过父边

            if (dTime[u] == 0) {  // 树边
                children++;
                dfs(u, v);
                low[v] = min(low[v], low[u]);

                // 桥判定
                if (low[u] > dTime[v])
                    bridges.push_back({v, u});

                // 割点判定
                if (parent != -1 && low[u] >= dTime[v])
                    isCut[v] = true;

            } else {  // 回边
                low[v] = min(low[v], dTime[u]);
            }
        }

        // 根节点的特殊判定
        if (parent == -1 && children > 1)
            isCut[v] = true;
    }

public:
    BridgesCutPoints(int n, const vector<vector<int>>& g) : n(n), graph(g) {}

    void find() {
        dTime.assign(n, 0);
        low.assign(n, 0);
        isCut.assign(n, false);
        bridges.clear();
        timer = 0;

        for (int v = 0; v < n; v++)
            if (dTime[v] == 0)
                dfs(v, -1);
    }

    const vector<pair<int, int>>& getBridges() const { return bridges; }
    const vector<bool>& getCutPoints() const { return isCut; }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  桥与割点 (Bridges & Articulation Points)" << endl;
    cout << "========================================" << endl << endl;

    // 示例无向图
    //    0---1---3
    //     \ /    |
    //      2     4
    int n = 5;
    vector<vector<int>> g(n);
    g[0].push_back(1); g[1].push_back(0);
    g[0].push_back(2); g[2].push_back(0);
    g[1].push_back(2); g[2].push_back(1);
    g[1].push_back(3); g[3].push_back(1);
    g[3].push_back(4); g[4].push_back(3);

    BridgesCutPoints bcp(n, g);
    bcp.find();

    cout << "[桥] (删除后图不连通的边):" << endl;
    auto bridges = bcp.getBridges();
    for (auto [u, v] : bridges)
        cout << "  " << u << " — " << v << endl;

    cout << endl << "[割点] (删除后图不连通的顶点):" << endl;
    auto cutPoints = bcp.getCutPoints();
    for (int v = 0; v < n; v++)
        if (cutPoints[v]) cout << "  顶点 " << v << endl;

    cout << endl << "[判定条件]" << endl;
    cout << "  桥:   low[u] > dTime[v]  (v→u 为树边)" << endl;
    cout << "  割点: low[u] ≥ dTime[v]  (非根)" << endl;
    cout << "  根割点: 有 ≥2 个孩子" << endl;

    return 0;
}
