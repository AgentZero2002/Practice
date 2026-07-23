/**
 * Prim 最小生成树算法
 *
 * 对应教材 6.8 节：最小支撑树
 *
 * 给定一个无向带权连通图，找出一棵权值之和最小的生成树。
 *
 * Prim 算法思想:
 *   从任意顶点开始，不断将距离已有树最近的顶点加入树中，
 *   直到所有顶点都在树内。
 *
 * 时间复杂度: O((V+E) log V) with 优先队列
 *            O(V²) with 朴素实现
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
using namespace std;

typedef pair<int, int> Edge;  // {权重, 目标顶点}

// ==================== Prim 算法 ====================
// 返回最小生成树的边和总权重
pair<vector<pair<int, int>>, int> primMST(const vector<vector<Edge>>& graph, int start = 0) {
    int n = graph.size();
    vector<bool> inMST(n, false);         // 是否已在最小生成树中
    vector<int> minWeight(n, INT_MAX);    // 到树的最短距离
    vector<int> parent(n, -1);            // 父节点

    // 优先队列: {权重, 顶点}
    priority_queue<Edge, vector<Edge>, greater<Edge>> pq;

    minWeight[start] = 0;
    pq.push({0, start});

    int totalWeight = 0;
    vector<pair<int, int>> mstEdges;  // {u, v}

    while (!pq.empty()) {
        auto [weight, v] = pq.top(); pq.pop();

        if (inMST[v]) continue;  // 过时条目
        inMST[v] = true;
        totalWeight += weight;

        if (parent[v] != -1)
            mstEdges.push_back({parent[v], v});

        // 松弛 v 的所有邻居
        for (auto& [w, u] : graph[v]) {
            if (!inMST[u] && w < minWeight[u]) {
                minWeight[u] = w;
                parent[u] = v;
                pq.push({w, u});
            }
        }
    }

    return {mstEdges, totalWeight};
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Prim 最小生成树算法                    " << endl;
    cout << "========================================" << endl << endl;

    // 无向带权图
    // 顶点 0..6，边 (u, v, weight)
    int n = 7;
    vector<vector<Edge>> graph(n);

    auto addUndirected = [&](int u, int v, int w) {
        graph[u].push_back({w, v});
        graph[v].push_back({w, u});
    };

    addUndirected(0, 1, 7);
    addUndirected(0, 3, 5);
    addUndirected(1, 2, 8);
    addUndirected(1, 3, 9);
    addUndirected(1, 4, 7);
    addUndirected(2, 4, 5);
    addUndirected(3, 4, 15);
    addUndirected(3, 5, 6);
    addUndirected(4, 5, 8);
    addUndirected(4, 6, 9);
    addUndirected(5, 6, 11);

    cout << "[无向带权图]" << endl;
    cout << "  7 个顶点, 11 条边" << endl << endl;

    auto [mstEdges, totalWeight] = primMST(graph, 0);

    cout << "[Prim 最小生成树]" << endl;
    cout << "  树边 (共 " << mstEdges.size() << " 条):" << endl;
    for (auto [u, v] : mstEdges)
        cout << "    " << u << " — " << v << endl;
    cout << "  总权重: " << totalWeight << endl;

    // 验证
    cout << endl << "[性质]" << endl;
    cout << "  1. MST 包含 n-1 = " << n-1 << " 条边 (实际 " << mstEdges.size() << ")" << endl;
    cout << "  2. MST 是连通的且无环" << endl;
    cout << "  3. Prim 和 Kruskal 都会产生 MST，但可能不是同一棵树" << endl;

    return 0;
}
