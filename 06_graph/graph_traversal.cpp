/**
 * 图的遍历：DFS（深度优先）和 BFS（广度优先）
 *
 * 对应教材 6.3-6.4 节：图的搜索
 *
 * DFS — Depth First Search
 *   核心: 沿着分支尽可能深入，无法继续时回退
 *   应用: 拓扑排序、连通分量、双向连通域分解
 *   性质: dTime(发现时间) 和 fTime(完成时间) 可对边分类
 *
 * BFS — Breadth First Search
 *   核心: 按距离分层访问，先访问离起点近的顶点
 *   应用: 最短路径（无权图）、连通分量
 *   性质: 生成 BFS 树，从起点到各顶点的最短距离
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
using namespace std;

// ==================== 邻接表图 ====================
class Graph {
private:
    int _n;
    vector<vector<int>> _adj;  // 邻接表（无权，存储邻居）
    vector<bool> _visited;

public:
    Graph(int n) : _n(n), _adj(n), _visited(n) {}

    void addEdge(int u, int v) {
        _adj[u].push_back(v);
    }

    void addUndirectedEdge(int u, int v) {
        _adj[u].push_back(v);
        _adj[v].push_back(u);
    }

    // ==================== BFS ====================
    // 从 start 开始 BFS，返回访问顺序和各顶点的距离
    vector<int> bfs(int start, vector<int>& distance) {
        vector<int> order;
        fill(_visited.begin(), _visited.end(), false);
        distance.assign(_n, -1);

        queue<int> q;
        q.push(start);
        _visited[start] = true;
        distance[start] = 0;

        while (!q.empty()) {
            int v = q.front(); q.pop();
            order.push_back(v);

            for (int u : _adj[v]) {
                if (!_visited[u]) {
                    _visited[u] = true;
                    distance[u] = distance[v] + 1;
                    q.push(u);
                }
            }
        }

        return order;
    }

    // ==================== DFS（递归版） ====================
    vector<int> dfs_recursive(int start) {
        vector<int> order;
        fill(_visited.begin(), _visited.end(), false);
        dfs_recur(start, order);

        // 处理不连通的顶点
        for (int v = 0; v < _n; v++)
            if (!_visited[v])
                dfs_recur(v, order);

        return order;
    }

    // ==================== DFS（迭代版，显式栈） ====================
    vector<int> dfs_iterative(int start) {
        vector<int> order;
        fill(_visited.begin(), _visited.end(), false);

        stack<int> S;
        S.push(start);

        while (!S.empty()) {
            int v = S.top(); S.pop();

            if (_visited[v]) continue;
            _visited[v] = true;
            order.push_back(v);

            // 将邻居逆序压入栈（保证与递归版顺序一致）
            for (auto it = _adj[v].rbegin(); it != _adj[v].rend(); ++it) {
                if (!_visited[*it])
                    S.push(*it);
            }
        }

        return order;
    }

private:
    void dfs_recur(int v, vector<int>& order) {
        _visited[v] = true;
        order.push_back(v);

        for (int u : _adj[v])
            if (!_visited[u])
                dfs_recur(u, order);
    }
};

// ==================== 辅助函数 ====================
void printOrder(const string& label, const vector<int>& order) {
    cout << "  " << label << ": ";
    for (size_t i = 0; i < order.size(); i++) {
        if (i > 0) cout << " → ";
        cout << order[i];
    }
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  图的遍历: DFS 与 BFS                   " << endl;
    cout << "========================================" << endl << endl;

    // 构建一个有向图
    //  0→1, 0→2, 1→3, 1→4, 2→5, 3→6, 4→6, 5→6
    Graph g(7);
    g.addEdge(0, 1); g.addEdge(0, 2);
    g.addEdge(1, 3); g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(3, 6); g.addEdge(4, 6); g.addEdge(5, 6);

    cout << "[测试图] 邻接表:" << endl;
    for (int v = 0; v < 7; v++) {
        cout << "  " << v << " → {2, 5}" << endl;  // placeholder
    }

    // BFS
    cout << endl << "[BFS 遍历] (从 0 开始)" << endl;
    vector<int> dist;
    auto bfsOrder = g.bfs(0, dist);
    printOrder("BFS 顺序", bfsOrder);
    cout << "  距离 from 0: ";
    for (int i = 0; i < 7; i++) cout << i << ":" << dist[i] << " ";
    cout << endl;

    // DFS 递归
    auto dfsRecur = g.dfs_recursive(0);
    printOrder("DFS 递归", dfsRecur);

    // DFS 迭代
    auto dfsIter = g.dfs_iterative(0);
    printOrder("DFS 迭代", dfsIter);

    // 对比
    cout << endl << "[DFS vs BFS]" << endl;
    cout << "  BFS: 逐层扩展，找到最短路径 | 队列 | 适合找最短路径" << endl;
    cout << "  DFS: 深入优先，回退后换路   | 栈   | 适合拓扑排序、连通域" << endl;
    cout << "  两者时间复杂度均为 O(V+E)" << endl;

    return 0;
}
