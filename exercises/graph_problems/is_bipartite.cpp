/**
 * 二分图判定 (Is Bipartite?)
 *
 * 经典题: LeetCode 785 — Is Graph Bipartite?
 *
 * 二分图: 可以将顶点分成两个独立集 U 和 V，
 *         使得每条边的一端在 U 中，另一端在 V 中。
 *
 * 判定方法: 染色法 (BFS/DFS)
 *   对每个连通分量:
 *   - 选一个未染色顶点染为色0
 *   - 将所有邻居染为相反色1
 *   - 检查邻居是否已染同色 → 若是，则不是二分图
 *
 * 时间复杂度 O(V+E)
 * 有趣结论: 图是二分图 ⟺ 不含奇数长度的环
 */

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// ==================== BFS 染色判定 ====================
bool isBipartite(const vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> color(n, -1);  // -1: 未染色, 0: 色A, 1: 色B

    for (int start = 0; start < n; start++) {
        if (color[start] != -1) continue;  // 已处理过

        queue<int> q;
        q.push(start);
        color[start] = 0;

        while (!q.empty()) {
            int v = q.front(); q.pop();

            for (int u : graph[v]) {
                if (color[u] == -1) {
                    color[u] = 1 - color[v];  // 染相反色
                    q.push(u);
                } else if (color[u] == color[v]) {
                    return false;  // 同色邻居 → 不是二分图
                }
            }
        }
    }

    return true;
}

// ==================== DFS 染色判定 ====================
bool dfsColor(const vector<vector<int>>& graph, int v,
              vector<int>& color, int c) {
    color[v] = c;
    for (int u : graph[v]) {
        if (color[u] == -1) {
            if (!dfsColor(graph, u, color, 1 - c))
                return false;
        } else if (color[u] == c) {
            return false;
        }
    }
    return true;
}

bool isBipartiteDFS(const vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> color(n, -1);
    for (int v = 0; v < n; v++)
        if (color[v] == -1 && !dfsColor(graph, v, color, 0))
            return false;
    return true;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  二分图判定 (染色法)                     " << endl;
    cout << "========================================" << endl << endl;

    // 测试1: 二分图
    // 0--1
    // |  |
    // 3--2
    vector<vector<int>> g1 = {
        {1, 3}, {0, 2}, {1, 3}, {2, 0}
    };
    cout << "[测试1] 四边形 (二分图)" << endl;
    cout << "  BFS 判定: " << (isBipartite(g1) ? "是二分图 ✓" : "不是 ✗") << endl;
    cout << "  DFS 判定: " << (isBipartiteDFS(g1) ? "是二分图 ✓" : "不是 ✗") << endl << endl;

    // 测试2: 含三角形 (非二分图)
    vector<vector<int>> g2 = {
        {1, 2}, {0, 2}, {0, 1}
    };
    cout << "[测试2] 三角形 (非二分图)" << endl;
    cout << "  BFS 判定: " << (isBipartite(g2) ? "是二分图 ✗" : "不是 ✓") << endl;
    cout << "  DFS 判定: " << (isBipartiteDFS(g2) ? "是二分图 ✗" : "不是 ✓") << endl;

    cout << endl << "[定理]" << endl;
    cout << "  图是二分图 ⟺ 不含奇长度环" << endl;
    cout << "  应用: 任务分配、婚姻匹配、考试排期" << endl;

    return 0;
}
