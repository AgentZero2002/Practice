/**
 * 图 ADT 实现（邻接矩阵 + 邻接表）
 *
 * 对应教材 第6章：图
 *
 * 两种经典表示:
 *   - AdjMatrix: 邻接矩阵 O(n²) 空间，O(1) 边查询
 *   - AdjList:   邻接表 O(n+e) 空间，适合稀疏图
 *
 * 同时提供 BFS、DFS 遍历框架
 */

#ifndef GRAPH_IMPL_HPP
#define GRAPH_IMPL_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
using namespace std;

// 边状态
enum VStatus { UNDISCOVERED, DISCOVERED, VISITED };
enum EType { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD };

// ==================== 邻接矩阵实现 ====================
class AdjMatrixGraph {
private:
    int _n;                                    // 顶点数
    vector<vector<int>> _adj;                  // 邻接矩阵（权重）
    vector<VStatus> _status;                    // 顶点状态
    vector<int> _dTime, _fTime;                // DFS 时间标签
    int _clock;

public:
    AdjMatrixGraph(int n) : _n(n),
        _adj(n, vector<int>(n, INT_MAX)),
        _status(n, UNDISCOVERED),
        _dTime(n), _fTime(n), _clock(0) {
        for (int i = 0; i < n; i++)
            _adj[i][i] = 0;  // 自环权重为0
    }

    int vertexCount() const { return _n; }

    void addEdge(int u, int v, int weight = 1) {
        _adj[u][v] = weight;
    }

    bool hasEdge(int u, int v) const {
        return _adj[u][v] != INT_MAX;
    }

    int weight(int u, int v) const { return _adj[u][v]; }

    // BFS 遍历
    void bfs(int start, vector<int>& order) {
        fill(_status.begin(), _status.end(), UNDISCOVERED);
        queue<int> Q;
        _status[start] = DISCOVERED;
        Q.push(start);

        while (!Q.empty()) {
            int v = Q.front(); Q.pop();
            order.push_back(v);
            for (int u = 0; u < _n; u++) {
                if (u != v && _adj[v][u] != INT_MAX &&
                    _status[u] == UNDISCOVERED) {
                    _status[u] = DISCOVERED;
                    Q.push(u);
                }
            }
            _status[v] = VISITED;
        }
    }

    // DFS 遍历（递归）
    void dfs(int start, vector<int>& order) {
        fill(_status.begin(), _status.end(), UNDISCOVERED);
        _clock = 0;
        dfsVisit(start, order);
        // 处理不连通的顶点
        for (int v = 0; v < _n; v++)
            if (_status[v] == UNDISCOVERED)
                dfsVisit(v, order);
    }

private:
    void dfsVisit(int v, vector<int>& order) {
        _status[v] = DISCOVERED;
        _dTime[v] = ++_clock;
        order.push_back(v);

        for (int u = 0; u < _n; u++) {
            if (u != v && _adj[v][u] != INT_MAX &&
                _status[u] == UNDISCOVERED) {
                dfsVisit(u, order);
            }
        }

        _status[v] = VISITED;
        _fTime[v] = ++_clock;
    }
};

// ==================== 邻接表实现 ====================
class AdjListGraph {
private:
    int _n;
    vector<vector<pair<int, int>>> _adj;  // {邻居, 权重}

public:
    AdjListGraph(int n) : _n(n), _adj(n) {}

    int vertexCount() const { return _n; }

    void addEdge(int u, int v, int weight = 1) {
        _adj[u].push_back({v, weight});
    }

    void addUndirectedEdge(int u, int v, int weight = 1) {
        addEdge(u, v, weight);
        addEdge(v, u, weight);
    }

    const vector<pair<int, int>>& neighbors(int u) const {
        return _adj[u];
    }

    // 判断 u→v 是否有边
    bool hasEdge(int u, int v) const {
        for (auto& p : _adj[u])
            if (p.first == v) return true;
        return false;
    }

    // 顶点 v 的出度
    int degree(int v) const { return _adj[v].size(); }
};

#endif // GRAPH_IMPL_HPP
