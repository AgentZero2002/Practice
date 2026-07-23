/**
 * Dijkstra 最短路径算法
 *
 * 对应教材 6.9 节：最短路径
 *
 * 求从源顶点 s 到图中所有其他顶点的最短路径。
 * 要求所有边的权重为非负。
 *
 * 算法思想:
 *   PFS (Priority-First Search) 框架的特例 —
 *   优先队列的优先级 = 当前已知最短距离
 *
 * 时间复杂度: O((V+E) log V)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
using namespace std;

typedef pair<int, int> Edge;  // {权重, 目标}

// ==================== Dijkstra 算法 ====================
// 返回: (最短距离数组, 前驱数组)
pair<vector<int>, vector<int>> dijkstra(const vector<vector<Edge>>& graph, int src) {
    int n = graph.size();
    vector<int> dist(n, INT_MAX);
    vector<int> prev(n, -1);

    // 小顶堆: {距离, 顶点}
    priority_queue<Edge, vector<Edge>, greater<Edge>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, v] = pq.top(); pq.pop();

        if (d > dist[v]) continue;  // 过时的条目（更短的路径已确定）

        // 松弛 v 的所有出边
        for (auto& [w, u] : graph[v]) {
            if (dist[v] + w < dist[u]) {
                dist[u] = dist[v] + w;
                prev[u] = v;
                pq.push({dist[u], u});
            }
        }
    }

    return {dist, prev};
}

// ==================== 回溯最短路径 ====================
// 从 src 到 dst 的最短路径（顶点序列）
vector<int> getPath(int src, int dst, const vector<int>& prev) {
    vector<int> path;
    if (prev[dst] == -1 && src != dst) return path;  // 不可达

    for (int v = dst; v != -1; v = prev[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Dijkstra 最短路径算法                  " << endl;
    cout << "========================================" << endl << endl;

    // 有向带权图（原教材示例）
    int n = 6;
    vector<vector<Edge>> graph(n);

    // 添加有向边
    auto addEdge = [&](int u, int v, int w) { graph[u].push_back({w, v}); };

    addEdge(0, 1, 10);
    addEdge(0, 3, 30);
    addEdge(0, 4, 100);
    addEdge(1, 2, 50);
    addEdge(2, 4, 10);
    addEdge(3, 2, 20);
    addEdge(3, 4, 60);

    cout << "[有向带权图] 6 个顶点" << endl << endl;

    int src = 0;
    auto [dist, prev] = dijkstra(graph, src);

    cout << "[从顶点 " << src << " 出发的最短路径]" << endl;
    cout << "  目标 | 最短距离 | 路径" << endl;
    cout << "  " << string(35, '-') << endl;

    for (int dst = 0; dst < n; dst++) {
        cout << "  " << dst << "    | ";
        if (dist[dst] == INT_MAX) {
            cout << "∞ (不可达)" << endl;
        } else {
            cout << setw(8) << dist[dst] << " | ";
            vector<int> path = getPath(src, dst, prev);
            for (size_t i = 0; i < path.size(); i++) {
                if (i > 0) cout << " → ";
                cout << path[i];
            }
            cout << endl;
        }
    }

    // 不可有负权边
    cout << endl << "[注意事项]" << endl;
    cout << "  1. Dijkstra 不允许负权边" << endl;
    cout << "  2. 若有负权边，请使用 Bellman-Ford 算法 O(VE)" << endl;
    cout << "  3. 若有负权环，最短路径不存在" << endl;
    cout << "  4. 这是 PFS (优先级优先搜索) 框架的特例" << endl;

    return 0;
}
