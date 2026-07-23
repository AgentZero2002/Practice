/**
 * 最小费用最大流 (Min-Cost Max-Flow)
 *
 * 在最大流的基础上, 每条边还有一个单位流量费用。
 * 目标: 在最大流的前提下, 最小化总费用。
 *
 * 算法: 连续最短路径 (Successive Shortest Path, SSP)
 *   1. 用 Bellman-Ford (或 SPFA) 在残量网络中找一条
 *      从源到汇的"最短路径" (按费用)
 *   2. 沿该路径增广最大可能的流量
 *   3. 重复直到无法到达汇点
 *
 * 关键: 由于残量网络中存在负费用边 (反向边),
 *   需要用 SPFA 或 Bellman-Ford (Dijkstra 不支持负边)。
 *
 * 优化: 用势函数 (Johnson's potentials) 将费用转为非负,
 *   之后可以用 Dijkstra 替代 SPFA → O(F · E log V)
 *
 * 应用:
 *   - 运输问题: 工厂 → 仓库, 最小化运输成本
 *   - 指派问题: n 个人分配 n 个任务 (等价于匈牙利算法)
 *   - 网络设计: 最小费用满足带宽需求
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
using namespace std;

const long long INF = 1e18;

struct Edge {
    int to, rev;
    long long cap, cost;
};

class MinCostMaxFlow {
private:
    int n;
    vector<vector<Edge>> g;

public:
    MinCostMaxFlow(int n) : n(n), g(n) {}

    void addEdge(int from, int to, long long cap, long long cost) {
        g[from].push_back({to, (int)g[to].size(), cap, cost});
        g[to].push_back({from, (int)g[from].size() - 1, 0, -cost});
    }

    // 返回 {最大流量, 最小费用}
    pair<long long, long long> solve(int s, int t) {
        long long flow = 0, cost = 0;

        while (true) {
            vector<long long> dist(n, INF);
            vector<int> prevNode(n, -1), prevEdge(n, -1);
            vector<bool> inQueue(n, false);

            queue<int> q;
            dist[s] = 0;
            q.push(s);
            inQueue[s] = true;

            // SPFA
            while (!q.empty()) {
                int u = q.front(); q.pop();
                inQueue[u] = false;

                for (int i = 0; i < (int)g[u].size(); i++) {
                    Edge& e = g[u][i];
                    if (e.cap > 0 && dist[e.to] > dist[u] + e.cost) {
                        dist[e.to] = dist[u] + e.cost;
                        prevNode[e.to] = u;
                        prevEdge[e.to] = i;
                        if (!inQueue[e.to]) {
                            q.push(e.to);
                            inQueue[e.to] = true;
                        }
                    }
                }
            }

            if (dist[t] == INF) break;  // 无增广路

            // 找瓶颈
            long long aug = INF;
            for (int v = t; v != s; v = prevNode[v])
                aug = min(aug, g[prevNode[v]][prevEdge[v]].cap);

            // 增广
            for (int v = t; v != s; v = prevNode[v]) {
                Edge& e = g[prevNode[v]][prevEdge[v]];
                e.cap -= aug;
                g[v][e.rev].cap += aug;
            }

            flow += aug;
            cost += aug * dist[t];
        }
        return {flow, cost};
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  最小费用最大流 (Min-Cost Max-Flow)        " << endl;
    cout << "========================================" << endl << endl;

    MinCostMaxFlow mcmf(5);
    // 源=0, 汇=4
    // s→1 (cap=2, cost=1), s→2 (cap=3, cost=2)
    // 1→3 (cap=3, cost=3), 2→3 (cap=2, cost=1)
    // 1→4 (cap=1, cost=4), 3→4 (cap=4, cost=2)
    mcmf.addEdge(0, 1, 2, 1);
    mcmf.addEdge(0, 2, 3, 2);
    mcmf.addEdge(1, 3, 3, 3);
    mcmf.addEdge(2, 3, 2, 1);
    mcmf.addEdge(1, 4, 1, 4);
    mcmf.addEdge(3, 4, 4, 2);

    auto [flow, cost] = mcmf.solve(0, 4);
    cout << "[MCMF]" << endl;
    cout << "  最大流: " << flow << endl;
    cout << "  最小费用: " << cost << endl;
    cout << endl;

    cout << "[算法]" << endl;
    cout << "  SPFA/SSP: 每次增广 O(VE), 共 F 次" << endl;
    cout << "  优化: 势函数 (Potentials) + Dijkstra → O(F E log V)" << endl;
    cout << "  更大的图: 预流推送 + 费用缩放" << endl;

    return 0;
}
