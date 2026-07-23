/**
 * 拓扑排序 (Topological Sort)
 *
 * 对应教材 6.6 节：拓扑排序
 *
 * 输入: 有向无环图 (DAG)
 * 输出: 顶点的线性序列，使得对每条边 u→v，u 在 v 之前出现
 *
 * 两种实现:
 *   1. Kahn 算法 (基于零入度) — BFS 风格
 *      反复找出入度为 0 的顶点并删除
 *
 *   2. DFS 后序逆序 — DFS 风格
 *      在 DFS 完成时记录顶点，最后反转即为拓扑序
 *      （教材中称为"零出度算法"）
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
using namespace std;

class Graph {
private:
    int _n;
    vector<vector<int>> _adj;

public:
    Graph(int n) : _n(n), _adj(n) {}

    void addEdge(int u, int v) { _adj[u].push_back(v); }
    int size() const { return _n; }
    const vector<int>& neighbors(int v) const { return _adj[v]; }

    // ==================== 方法一: Kahn 算法 (BFS) ====================
    // 反复找出入度为 0 的顶点
    vector<int> topologicalSortKahn() {
        // 计算各顶点入度
        vector<int> indegree(_n, 0);
        for (int v = 0; v < _n; v++)
            for (int u : _adj[v])
                indegree[u]++;

        // 入度为 0 的顶点入队
        queue<int> q;
        for (int v = 0; v < _n; v++)
            if (indegree[v] == 0)
                q.push(v);

        vector<int> order;
        while (!q.empty()) {
            int v = q.front(); q.pop();
            order.push_back(v);

            // 删除 v 及其出边
            for (int u : _adj[v]) {
                if (--indegree[u] == 0)
                    q.push(u);
            }
        }

        // 若 order 长度 < n，说明存在环（不是 DAG）
        return order;
    }

    // ==================== 方法二: DFS 后序逆序 ====================
    // 在 DFS 回溯时记录顶点，最后反转
    vector<int> topologicalSortDFS() {
        vector<bool> visited(_n, false);
        vector<bool> onPath(_n, false);  // 检测环
        vector<int> order;

        bool hasCycle = false;
        for (int v = 0; v < _n; v++) {
            if (!visited[v]) {
                dfsForTopo(v, visited, onPath, order, hasCycle);
                if (hasCycle) return {};  // 存在环
            }
        }

        reverse(order.begin(), order.end());  // 后序逆序 = 拓扑排序
        return order;
    }

private:
    void dfsForTopo(int v, vector<bool>& visited, vector<bool>& onPath,
                    vector<int>& order, bool& hasCycle) {
        visited[v] = true;
        onPath[v] = true;

        for (int u : _adj[v]) {
            if (!visited[u]) {
                dfsForTopo(u, visited, onPath, order, hasCycle);
            } else if (onPath[u]) {
                hasCycle = true;  // 发现后向边 → 存在环
                return;
            }
        }

        onPath[v] = false;
        order.push_back(v);  // 在 DFS 回溯时记录
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  拓扑排序 (Topological Sort)           " << endl;
    cout << "========================================" << endl << endl;

    // 示例: 课程依赖关系
    // 0: C语言, 1: 离散数学, 2: 数据结构, 3: 算法, 4: OS, 5: 编译原理
    // 依赖: 2→3, 0→2, 1→2, 2→4, 3→4, 0→5, 2→5, 1→5, 4→5
    Graph g(6);
    g.addEdge(2, 3);   // 数据结构 → 算法
    g.addEdge(0, 2);   // C语言 → 数据结构
    g.addEdge(1, 2);   // 离散数学 → 数据结构
    g.addEdge(2, 4);   // 数据结构 → OS
    g.addEdge(3, 4);   // 算法 → OS
    g.addEdge(0, 5);   // C语言 → 编译原理
    g.addEdge(2, 5);   // 数据结构 → 编译原理
    g.addEdge(1, 5);   // 离散数学 → 编译原理
    g.addEdge(4, 5);   // OS → 编译原理

    string names[] = {"C语言", "离散数学", "数据结构", "算法", "OS", "编译原理"};

    cout << "[课程依赖 DAG]" << endl;
    cout << "  C语言 ──→ 数据结构 ──→ 算法" << endl;
    cout << "    │          │    \\      │" << endl;
    cout << "    │          │     \\     ↓" << endl;
    cout << "    │          │      →→  OS" << endl;
    cout << "    │          │           │" << endl;
    cout << "    │          │           ↓" << endl;
    cout << "    └──→ 编译原理 ←───┘" << endl;
    cout << "   离散数学 ──→ 编译原理" << endl << endl;

    // Kahn 算法
    auto kahn = g.topologicalSortKahn();
    cout << "[Kahn 算法]" << endl;
    cout << "  拓扑序: ";
    for (int v : kahn) cout << names[v] << " → ";
    cout << "END" << endl;
    cout << "  长度: " << kahn.size() << " (是否为 DAG: "
         << (kahn.size() == 6 ? "✓" : "✗") << ")" << endl << endl;

    // DFS 版本
    auto dfs = g.topologicalSortDFS();
    cout << "[DFS 后序逆序]" << endl;
    cout << "  拓扑序: ";
    for (int v : dfs) cout << names[v] << " → ";
    cout << "END" << endl;
    cout << "  长度: " << dfs.size() << " (是否为 DAG: "
         << (dfs.size() == 6 ? "✓" : "✗") << ")" << endl;

    return 0;
}
