/**
 * AOE 网关键路径 (Critical Path)
 *
 * 对应教材 6.11 节：关键路径
 *
 * AOE (Activity On Edge) 网:
 *   - 顶点表示事件 (Event)
 *   - 边表示活动 (Activity)，权重 = 持续时间
 *
 * 关键路径:
 *   从源点到汇点的最长路径，决定整个工程的工期。
 *   关键路径上的活动不能延迟，否则整体工期延长。
 *
 * 算法:
 *   1. 拓扑排序，计算各事件的最早发生时间 ve[i]
 *   2. 逆拓扑序，计算各事件的最迟发生时间 vl[i]
 *   3. 对每条活动 (i,j)，若 ve[i] == vl[j]-weight，则该活动为关键活动
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
using namespace std;

struct Edge {
    int to, weight;
};

// ==================== 关键路径分析 ====================
void criticalPathAnalysis(int n, const vector<vector<Edge>>& graph,
                          int source, int sink) {
    // Step 1: 拓扑排序
    vector<int> indegree(n, 0);
    for (int v = 0; v < n; v++)
        for (auto& e : graph[v])
            indegree[e.to]++;

    queue<int> q;
    vector<int> topoOrder;
    for (int v = 0; v < n; v++)
        if (indegree[v] == 0) q.push(v);

    while (!q.empty()) {
        int v = q.front(); q.pop();
        topoOrder.push_back(v);
        for (auto& e : graph[v])
            if (--indegree[e.to] == 0)
                q.push(e.to);
    }

    if ((int)topoOrder.size() != n) {
        cout << "  图中有环，不是 DAG!" << endl;
        return;
    }

    // Step 2: 计算最早发生时间 ve (顺拓扑序)
    vector<int> ve(n, 0);
    for (int v : topoOrder)
        for (auto& e : graph[v])
            ve[e.to] = max(ve[e.to], ve[v] + e.weight);

    // Step 3: 计算最迟发生时间 vl (逆拓扑序)
    vector<int> vl(n, INT_MAX);
    vl[sink] = ve[sink];  // 终点: 最迟 = 最早
    for (auto it = topoOrder.rbegin(); it != topoOrder.rend(); ++it) {
        int v = *it;
        for (auto& e : graph[v])
            vl[v] = min(vl[v], vl[e.to] - e.weight);
    }

    // Step 4: 找出关键活动
    cout << "[事件时间]" << endl;
    cout << "  事件 | ve(最早) | vl(最迟) | 松弛" << endl;
    cout << "  " << string(35, '-') << endl;
    for (int i = 0; i < n; i++)
        cout << "  " << i << "    | " << setw(8) << ve[i]
             << " | " << setw(8) << vl[i]
             << " | " << (vl[i] - ve[i]) << endl;

    cout << endl << "[关键活动] (松弛为 0 的活动)" << endl;
    cout << "  边     | 权重 | 最早开始 | 最迟开始 | 关键" << endl;
    cout << "  " << string(45, '-') << endl;

    vector<Edge> criticalEdges;
    for (int v = 0; v < n; v++) {
        for (auto& e : graph[v]) {
            int slack = vl[e.to] - ve[v] - e.weight;  // 松弛时间
            bool critical = (slack == 0);
            if (critical)
                criticalEdges.push_back({v, e.to});
            cout << "  " << v << " → " << e.to << " | "
                 << setw(4) << e.weight << " | "
                 << setw(8) << ve[v] << " | "
                 << setw(8) << (vl[e.to] - e.weight) << " | "
                 << (critical ? "★" : "") << endl;
        }
    }

    cout << endl << "[关键路径] 总工期 = " << ve[sink] << endl;
    cout << "  关键路径上的活动 (不能延迟): ";
    for (size_t i = 0; i < criticalEdges.size(); i++) {
        if (i > 0) cout << " → ";
        cout << criticalEdges[i].to;
    }
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  AOE 网 — 关键路径分析                  " << endl;
    cout << "========================================" << endl << endl;

    // 经典 AOE 网：9 个事件，源点=0，汇点=8
    int n = 9;
    vector<vector<Edge>> graph(n);

    graph[0].push_back({1, 6}); graph[0].push_back({2, 4}); graph[0].push_back({3, 5});
    graph[1].push_back({4, 1});
    graph[2].push_back({4, 1});
    graph[3].push_back({5, 2});
    graph[4].push_back({6, 9}); graph[4].push_back({7, 7});
    graph[5].push_back({7, 4});
    graph[6].push_back({8, 2});
    graph[7].push_back({8, 4});

    cout << "[AOE 网] 顶点=事件, 边=活动(权重=持续时间)" << endl;
    cout << "  源点: 0, 汇点: 8" << endl << endl;

    criticalPathAnalysis(n, graph, 0, 8);

    return 0;
}
