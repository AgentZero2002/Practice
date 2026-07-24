/**
 * 最大流问题的线性规划形式
 *
 * 从 LP 视角理解最大流和最小割的对偶关系。
 *
 * 最大流 LP (Primal):
 *   变量: f_e 每条边的流量
 *   max  Σ_{e∈out(s)} f_e
 *   s.t. 0 ≤ f_e ≤ c_e                     (容量约束)
 *        Σ_{e∈in(v)} f_e = Σ_{e∈out(v)} f_e  (流量守恒, v≠s,t)
 *
 * 最小割 LP (Dual):
 *   变量: d_v ∈ {0,1} (v∈S 则 d_v=1)
 *         y_e ≥ 0 (边穿越割的指示)
 *   min  Σ_e c_e · y_e
 *   s.t. y_e ≥ d_u - d_v   (若 u∈S, v∉S 则 y_{uv}=1)
 *        d_s = 1, d_t = 0
 *
 * 弱对偶: 任意可行流 ≤ 任意可行割容量
 * 强对偶 (Max-Flow Min-Cut Theorem):
 *   最优流值 = 最小割容量
 *   即对偶 LP 的最优解是整数 (0/1), 无需显式的整数约束!
 *
 * 为什么 MFMC 是 LP 对偶的推论?
 *   因为约束矩阵是 totally unimodular (全幺模),
 *   所有极点解自动为整数。
 *
 * 互补松弛条件 (Complementary Slackness):
 *   最优解处:
 *     1. f_e < c_e ⇒ no pressure to increase (y_e = 0? 不对, 有方向)
 *     2. 流的残量网络上, s 可达的节点 = 最小割的 S 集
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
using namespace std;

const int INF = numeric_limits<int>::max();

// ==================== Dinic 最大流 (回顾) ====================
struct Edge {
    int to, rev, cap;
};

class Dinic {
    vector<vector<Edge>> g;
    vector<int> level, iter;

    void bfs(int s) {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (auto& e : g[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
    }

    int dfs(int v, int t, int f) {
        if (v == t) return f;
        for (int& i = iter[v]; i < (int)g[v].size(); i++) {
            Edge& e = g[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
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
    Dinic(int n) : g(n), level(n), iter(n) {}

    void addEdge(int from, int to, int cap) {
        g[from].push_back({to, (int)g[to].size(), cap});
        g[to].push_back({from, (int)g[from].size() - 1, 0});
    }

    int maxFlow(int s, int t) {
        int flow = 0;
        while (true) {
            bfs(s);
            if (level[t] < 0) break;
            fill(iter.begin(), iter.end(), 0);
            int f;
            while ((f = dfs(s, t, INF)) > 0) flow += f;
        }
        return flow;
    }

    // 从 s 可达的节点集合 = 最小割的 S 集
    vector<bool> minCutSet(int s) {
        bfs(s);
        vector<bool> reachable(g.size());
        for (int i = 0; i < (int)g.size(); i++)
            reachable[i] = (level[i] >= 0);
        return reachable;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  最大流 LP 与对偶性                       " << endl;
    cout << "========================================" << endl << endl;

    // s=0, t=5
    Dinic dinic(6);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 5);
    dinic.addEdge(1, 2, 2);
    dinic.addEdge(2, 4, 10);
    dinic.addEdge(3, 5, 10);
    dinic.addEdge(4, 3, 5);
    dinic.addEdge(4, 5, 10);

    int flow = dinic.maxFlow(0, 5);
    auto minCut = dinic.minCutSet(0);

    cout << "[Max Flow = Min Cut]" << endl;
    cout << "  最大流值: " << flow << endl;
    cout << "  最小割 S 集: ";
    for (int i = 0; i < 6; i++)
        if (minCut[i]) cout << i << " ";
    cout << endl;

    cout << endl << "[LP 对偶视角]" << endl;
    cout << "  Primal:  max Σ f_sv          (最大流)" << endl;
    cout << "  Dual:    min Σ c_e · y_e     (最小割)" << endl;
    cout << "  强对偶:  最优 Primal = 最优 Dual (= " << flow << ")" << endl;

    cout << endl << "[为什么是整数解?]" << endl;
    cout << "  约束矩阵是全幺模 (Totally Unimodular)" << endl;
    cout << "  所有基本可行解自动为整数" << endl;
    cout << "  无需显式添加 0/1 整数约束!" << endl;

    cout << endl << "[互补松弛]" << endl;
    cout << "  y_e > 0 ⇒ f_e = c_e  (割边饱和)" << endl;
    cout << "  f_e < c_e ⇒ y_e = 0  (非饱和边不在割中)" << endl;

    return 0;
}
