/**
 * 谱图论: PageRank + 谱聚类
 *
 * PageRank (Google 排名算法):
 *   网页的重要度 = 链接到它的其他网页的重要度之和 (加权)。
 *   随机游走视角: 冲浪者随机点链接, 也随机跳转到任意页面 (teleport)。
 *
 *   迭代公式:
 *     PR(p) = (1-d)/N + d × Σ PR(q)/outDeg(q)  (对所有 q→p)
 *     其中 d = damping factor (通常 0.85)
 *
 *   矩阵形式:
 *     PR = (1-d)/N · 1 + d · M^T · PR
 *     稳态分布 = M^T 矩阵的主特征向量
 *
 * 谱聚类 (Spectral Clustering):
 *   用图的 Laplacian 矩阵的特征向量进行聚类。
 *   关键思想: Laplacian 的第二小特征值对应的特征向量 (Fiedler向量)
 *   给出了图的"自然二分"。
 *
 *   步骤:
 *     1. 构建相似度矩阵 W
 *     2. 计算图 Laplacian: L = D - W (或 normalized: D⁻¹/² L D⁻¹/²)
 *     3. 计算 L 的前 k 个特征向量
 *     4. 将每个结点表示为特征向量组成的 k 维点
 *     5. 用 k-means 聚类
 *
 * 应用:
 *   - PageRank: Google 搜索, 社交网络影响力
 *   - 谱聚类: 图像分割, 社区发现, 降维
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

// ==================== PageRank 简化实现 ====================
vector<double> pageRank(const vector<vector<int>>& graph,
                        double damping = 0.85, int iters = 50) {
    int n = graph.size();
    vector<double> rank(n, 1.0 / n);

    // 计算出度
    vector<int> outDeg(n, 0);
    for (int u = 0; u < n; u++)
        outDeg[u] = graph[u].size();

    for (int iter = 0; iter < iters; iter++) {
        vector<double> newRank(n, (1.0 - damping) / n);

        for (int u = 0; u < n; u++) {
            if (outDeg[u] == 0) {
                // Dangling node: 均匀分配
                for (int v = 0; v < n; v++)
                    newRank[v] += damping * rank[u] / n;
            } else {
                for (int v : graph[u])
                    newRank[v] += damping * rank[u] / outDeg[u];
            }
        }
        rank = newRank;
    }
    return rank;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  谱图论 — PageRank + 谱聚类               " << endl;
    cout << "========================================" << endl << endl;

    // 简单的网页链接图: 0→1, 0→2, 1→2, 2→0
    vector<vector<int>> graph = {
        {1, 2},  // 网页 0 链接到 1,2
        {2},     // 网页 1 链接到 2
        {0},     // 网页 2 链接到 0
        {0, 1}   // 网页 3 链接到 0,1
    };

    auto pr = pageRank(graph);
    cout << "[PageRank] 4 个网页, damping=0.85" << endl;
    for (int i = 0; i < (int)pr.size(); i++)
        cout << "  网页" << i << ": PR=" << pr[i] << endl;

    // 找最高排名
    int best = max_element(pr.begin(), pr.end()) - pr.begin();
    cout << "  最高排名: 网页" << best << " (PR=" << pr[best] << ")" << endl;

    cout << endl << "[谱聚类思想]" << endl;
    cout << "  Laplacian L = D - W" << endl;
    cout << "  Fiedler 向量 (λ₂): 图的自然二分" << endl;
    cout << "  降维: 前 k 个特征向量 → k-means" << endl;
    cout << "  应用: 图像分割 (NCut), 社区发现" << endl;

    return 0;
}
