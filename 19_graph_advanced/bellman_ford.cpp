/**
 * Bellman-Ford 最短路径算法（含负权边）
 *
 * Dijkstra 无法处理负权边，Bellman-Ford 弥补了这一缺陷。
 *
 * 算法: 对图中所有边进行 V-1 轮松弛操作。
 *   第 k 轮结束后，dist[v] 为从源点出发、经过不超过 k 条边的最短距离。
 *
 * 负权环检测:
 *   再做第 V 轮松弛，如果还能更新 → 存在负权环（最短路径无下界）
 *
 * 时间复杂度: O(VE)
 * 空间复杂度: O(V)
 *
 * 相关算法:
 *   - SPFA (Shortest Path Faster Algorithm):
 *     Bellman-Ford 的队列优化版，平均更快但最坏仍为 O(VE)
 */

#include <iostream>
#include <vector>
#include <climits>
using namespace std;

const int INF = INT_MAX / 2;

struct Edge {
    int from, to, weight;
};

// ==================== Bellman-Ford ====================
// 返回 {最短距离, 是否存在负权环}
pair<vector<int>, bool> bellmanFord(int n, const vector<Edge>& edges, int src) {
    vector<int> dist(n, INF);
    dist[src] = 0;

    // 进行 n-1 轮松弛
    for (int round = 0; round < n - 1; round++) {
        bool updated = false;
        for (const Edge& e : edges) {
            if (dist[e.from] != INF && dist[e.from] + e.weight < dist[e.to]) {
                dist[e.to] = dist[e.from] + e.weight;
                updated = true;
            }
        }
        if (!updated) break;  // 提前终止：没有更新
    }

    // 第 n 轮：检测负权环
    bool hasNegativeCycle = false;
    for (const Edge& e : edges) {
        if (dist[e.from] != INF && dist[e.from] + e.weight < dist[e.to]) {
            hasNegativeCycle = true;
            break;
        }
    }

    return {dist, hasNegativeCycle};
}

// ==================== SPFA (队列优化版) ====================
vector<int> spfa(int n, const vector<vector<pair<int, int>>>& graph, int src) {
    vector<int> dist(n, INF);
    vector<bool> inQueue(n, false);
    vector<int> cnt(n, 0);  // 入队列次数检测负环

    dist[src] = 0;
    vector<int> q;  // 用 vector 模拟队列
    q.push_back(src);
    inQueue[src] = true;

    for (size_t head = 0; head < q.size() && head < (size_t)(n * n); head++) {
        int v = q[head];
        inQueue[v] = false;

        for (auto [weight, u] : graph[v]) {
            if (dist[v] + weight < dist[u]) {
                dist[u] = dist[v] + weight;
                if (!inQueue[u]) {
                    q.push_back(u);
                    inQueue[u] = true;
                    if (++cnt[u] > n) return {};  // 负环检测
                }
            }
        }
    }

    return dist;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Bellman-Ford 最短路径 (含负权边)       " << endl;
    cout << "========================================" << endl << endl;

    // 示例图（含负权边但无负环）
    int n = 5;
    vector<Edge> edges = {
        {0, 1, 6}, {0, 2, 7},
        {1, 2, 8}, {1, 3, 5}, {1, 4, -4},
        {2, 3, -3}, {2, 4, 9},
        {3, 1, -2},
        {4, 3, 7}
    };

    cout << "[图 1: 含负权边但无负环]" << endl;
    auto [dist, hasNegCycle] = bellmanFord(n, edges, 0);

    cout << "  从 0 出发的最短距离:" << endl;
    for (int v = 0; v < n; v++)
        cout << "    0 → " << v << ": " << (dist[v] >= INF/2 ? "∞" : to_string(dist[v])) << endl;
    cout << "  负权环: " << (hasNegCycle ? "存在!" : "不存在 ✓") << endl << endl;

    // 图2: 含负权环
    cout << "[图 2: 含负权环]" << endl;
    vector<Edge> edges2 = {
        {0, 1, 1}, {1, 2, -1}, {2, 0, -1}, {2, 3, 3}
    };
    auto [dist2, hasCycle] = bellmanFord(4, edges2, 0);
    cout << "  0→1→2→0: 1-1-1=-1 → 总权重为负!" << endl;
    cout << "  负权环: " << (hasCycle ? "存在! (最短路径无下界)" : "不存在") << endl << endl;

    cout << "[Dijkstra vs Bellman-Ford]" << endl;
    cout << "  Dijkstra:     O((V+E)logV), 不允许负权" << endl;
    cout << "  Bellman-Ford: O(VE), 允许负权, 可检测负环" << endl;
    cout << "  SPFA:         O(kE) 平均, 最坏 O(VE)" << endl;

    return 0;
}
