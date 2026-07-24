/**
 * 图上随机游走 (Random Walk on Graphs)
 *
 * 随机游走是谱图论的核心视角: 把 PageRank、谱聚类、电阻网络
 * 统一在"随机过程"的框架下理解。
 *
 * 定义:
 *   从节点 u 出发, 等概率走到任意邻居:
 *     P(u → v) = 1/deg(u)  (若 (u,v) ∈ E)
 *   转移矩阵: P = D^(-1) · W
 *
 * 平稳分布 (Stationary Distribution):
 *   无向连通图上的简单随机游走是 reversible Markov chain,
 *   平稳分布 π 满足 πP = π, 且
 *     π(v) = deg(v) / (2|E|)
 *   即: 度越大的节点, 在平稳状态下被访问的概率越大。
 *
 * 击中时间 (Hitting Time) H(u,v):
 *   从 u 出发首次到达 v 的期望步数。
 *
 * 往返时间 (Commute Time) C(u,v):
 *   C(u,v) = H(u,v) + H(v,u)  (从 u 到 v 再返回的期望步数)
 *
 * 覆盖时间 (Cover Time):
 *   从某点出发访问所有节点的期望步数。
 *   Alec's bound: cover time ≤ 2|E|(n-1)  (无向图)
 *
 * 与 Laplacian 的关系:
 *   C(u,v) = 2|E| · R_eff(u,v)
 *   其中 R_eff 是 effective resistance (见 electrical_network.cpp)
 *
 * 应用:
 *   - PageRank 是带 teleport 的随机游走
 *   - 社群检测: random walk 趋于停留在社群内部
 *   - 推荐系统: 随机游走 simrank
 */

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <unordered_map>
using namespace std;

// ==================== 简单随机游走模拟 ====================
class RandomWalk {
    const vector<vector<int>>& graph;
    mt19937 rng;
    int current;

public:
    RandomWalk(const vector<vector<int>>& g, int start = 0)
        : graph(g), rng(42), current(start) {}

    // 走一步, 返回新位置
    int step() {
        const auto& neighbors = graph[current];
        if (neighbors.empty()) return current;  // 孤立点
        uniform_int_distribution<int> dist(0, neighbors.size() - 1);
        current = neighbors[dist(rng)];
        return current;
    }

    int position() const { return current; }
    void reset(int node) { current = node; }
};

// ==================== 估计平稳分布 ====================
vector<double> stationaryDistribution(const vector<vector<int>>& graph,
                                       int steps = 100000, int burnIn = 1000) {
    int n = graph.size();
    vector<int> visitCount(n, 0);
    RandomWalk rw(graph);
    mt19937& rng = rw.rng;  // 用同一个 generator 没关系

    // Burn-in: 让链到达平稳状态
    for (int i = 0; i < burnIn; i++) rw.step();

    // 采样
    for (int i = 0; i < steps; i++) {
        visitCount[rw.step()]++;
    }

    vector<double> pi(n);
    for (int i = 0; i < n; i++)
        pi[i] = (double)visitCount[i] / steps;
    return pi;
}

// ==================== 估计击中时间 ====================
double estimateHittingTime(const vector<vector<int>>& graph,
                            int source, int target, int trials = 500) {
    mt19937 rng(123);
    long long totalSteps = 0;
    int n = graph.size();

    for (int t = 0; t < trials; t++) {
        int current = source;
        int steps = 0;
        while (current != target && steps < 10000) {
            const auto& nb = graph[current];
            if (nb.empty()) break;
            uniform_int_distribution<int> dist(0, nb.size() - 1);
            current = nb[dist(rng)];
            steps++;
        }
        totalSteps += steps;
    }
    return (double)totalSteps / trials;
}

// ==================== 计算理论平稳分布 ====================
vector<double> theoreticalStationary(const vector<vector<int>>& graph) {
    int n = graph.size();
    int totalDeg = 0;
    for (int i = 0; i < n; i++) totalDeg += graph[i].size();

    vector<double> pi(n);
    for (int i = 0; i < n; i++)
        pi[i] = (double)graph[i].size() / totalDeg;
    return pi;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  图上随机游走 (Random Walk)               " << endl;
    cout << "========================================" << endl << endl;

    // 棒球图: 0—1—2—3—4 (路径图, 5节点)
    vector<vector<int>> graph(5);
    graph[0] = {1};
    graph[1] = {0, 2};
    graph[2] = {1, 3};
    graph[3] = {2, 4};
    graph[4] = {3};

    cout << "[图结构] 路径图 0—1—2—3—4" << endl << endl;

    // 1. 平稳分布
    auto pi_theory = theoreticalStationary(graph);
    auto pi_sim = stationaryDistribution(graph);

    cout << "[平稳分布]  理论         模拟 (100K步)" << endl;
    for (int i = 0; i < 5; i++)
        cout << "  节点" << i << ":     "
             << fixed << setprecision(4) << pi_theory[i]
             << "         " << pi_sim[i] << endl;

    // 2. 击中时间
    cout << endl << "[击中时间估计] 每对500次模拟" << endl;
    for (int src = 0; src < 3; src++) {
        int tgt = src + 2;
        double h = estimateHittingTime(graph, src, tgt);
        cout << "  H(" << src << "→" << tgt << ") ≈ " << h << " 步" << endl;
    }

    cout << endl << "[随机游走性质]" << endl;
    cout << "  平稳分布 ∝ 节点度数" << endl;
    cout << "  C(u,v) = 2|E| × R_eff(u,v)  ← 与电阻网络的联系" << endl;
    cout << "  混合时间: ~1/λ₂  (代数连通度的倒数)" << endl;
    cout << "  PageRank = 带阻尼的随机游走平稳分布" << endl;

    return 0;
}
