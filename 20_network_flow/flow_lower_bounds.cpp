/**
 * 网络流高级专题 — 带下界的流 (Flow with Lower Bounds)
 *
 * 实际网络流问题中, 有些边不仅要求流量 ≤ 容量 (upper bound),
 * 还要求流量 ≥ 下界 (lower bound)。
 *
 * 问题分类:
 *
 * 1. 可行流 (Feasible Flow):
 *    每条边: l(e) ≤ f(e) ≤ c(e)
 *    ∀v: Σ f(in) = Σ f(out)  (流量守恒)
 *    求是否存在满足下界的可行流。
 *
 * 2. 带下界的最大流:
 *    先找可行流, 再在残量网络上增广。
 *
 * 3. 带下界的最小费用流
 *
 * 解决方案 (消除下界):
 *   对每条边 e(u→v) 有 l ≤ f ≤ c:
 *     - 边容量改为 c-l (新的上界)
 *     - 结点 u 的供需: demand[u] -= l (净流出多 l)
 *     - 结点 v 的供需: demand[v] += l (净流入多 l)
 *   然后添加超级源汇 S*, T*:
 *     - 对 demand[v] > 0: S* → v, cap = demand[v]
 *     - 对 demand[v] < 0: v → T*, cap = -demand[v]
 *   如果 S*→T* 的最大流 = Σ正demand, 则存在可行流
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
using namespace std;

const int INF = 1e9;

// 基础 Dinic (简化, 用于 net flow 检查)
class Dinic {
    int n;
    struct Edge { int to, rev, cap; };
    vector<vector<Edge>> g;
    vector<int> level, iter;

    void bfs(int s) {
        level.assign(n, -1);
        queue<int> q;
        level[s] = 0; q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto& e : g[u])
                if (e.cap > 0 && level[e.to] == -1) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
        }
    }

    int dfs(int u, int t, int f) {
        if (u == t) return f;
        for (int& i = iter[u]; i < (int)g[u].size(); i++) {
            Edge& e = g[u][i];
            if (e.cap > 0 && level[u] < level[e.to]) {
                int d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    g[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }

public:
    Dinic(int n) : n(n), g(n) {}

    void addEdge(int from, int to, int cap) {
        g[from].push_back({to, (int)g[to].size(), cap});
        g[to].push_back({from, (int)g[from].size() - 1, 0});
    }

    int maxFlow(int s, int t) {
        int flow = 0;
        while (true) {
            bfs(s);
            if (level[t] == -1) break;
            iter.assign(n, 0);
            int f;
            while ((f = dfs(s, t, INF)) > 0) flow += f;
        }
        return flow;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  带下界的流 (Flow with Lower Bounds)      " << endl;
    cout << "========================================" << endl << endl;

    cout << "[消除下界转换]" << endl;
    cout << "  原边 (l ≤ f ≤ c) → 新边 (0 ≤ f' ≤ c-l)" << endl;
    cout << "  demand[u] -= l, demand[v] += l" << endl;
    cout << "  添加 S*→v (cap=正demand), u→T* (cap=负demand)" << endl;
    cout << "  S*→T* 最大流 = 所有正demand之和 → 存在可行流" << endl;
    cout << endl;

    cout << "[代码示意]" << endl;
    cout << "  struct Edge { int l, r, cost; };" << endl;
    cout << "  // 每条边 x→transform→x' (capacity=r-l)" << endl;
    cout << "  // 添加 SS, TT, 跑 SS→TT maxflow" << endl;
    cout << "  // flow == sumPosDemand → feasible!" << endl;
    cout << endl;

    cout << "[应用]" << endl;
    cout << "  - 循环流 (Circulation)" << endl;
    cout << "  - 有源汇的上下界流: 加 t→s (capacity=INF)" << endl;
    cout << "  - 最大流/最小费用扩展: feasible后继续增广" << endl;

    return 0;
}
